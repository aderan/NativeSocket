package com.herewhite.sdk.nativesocket;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import android.widget.TextView;

import com.herewhite.sdk.rtns.RtnsSocketConfProvider;
import com.herewhite.sdk.rtns.RtnsSocketFactory;

import java.net.InetSocketAddress;
import java.net.SocketAddress;

import androidx.appcompat.app.AppCompatActivity;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.WebSocket;
import okhttp3.WebSocketListener;
import okio.ByteString;

/**
 * @author fenglibin
 */
public class MainActivity extends AppCompatActivity {
    OkHttpClient client;
    TextView tv;
    WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initClient();

        // Example of a call to a native method
        tv = findViewById(R.id.sample_text);
        tv.setMovementMethod(ScrollingMovementMethod.getInstance());

        webView = findViewById(R.id.webview);

        findViewById(R.id.closeWeb).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });
        findViewById(R.id.test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testWebSocket();
            }

            private void testWebSocket() {
                Request request = new Request.Builder().url("ws://121.196.198.83/echo").build();
                WebSocket ws = client.newWebSocket(request, new WebSocketListener() {
                    @Override
                    public void onOpen(WebSocket webSocket, Response response) {
                        webSocket.send("Hello, it's SSaurel !");
                        webSocket.send("What's up ?");
                        webSocket.send(ByteString.decodeHex("deadbeef"));

                        Log.e("MainActivity", "onOpen");
                        webSocket.close(1000, "Goodbye !");
                    }

                    @Override
                    public void onMessage(WebSocket webSocket, String text) {
                        Log.e("MainActivity", "onMessage");
                        output(text);
                    }

                    @Override
                    public void onMessage(WebSocket webSocket, ByteString bytes) {
                        output(bytes.toString());
                    }

                    @Override
                    public void onClosing(WebSocket webSocket, int code, String reason) {
                        super.onClosing(webSocket, code, reason);
                        webSocket.close(1000, null);
                        output("Closing : " + code + " / " + reason);
                    }

                    @Override
                    public void onClosed(WebSocket webSocket, int code, String reason) {
                        super.onClosed(webSocket, code, reason);
                        output("onClosed : " + code + " / " + reason);
                        Log.e("MainActivity", "onClosed");
                    }

                    @Override
                    public void onFailure(WebSocket webSocket, Throwable t, Response response) {
                        super.onFailure(webSocket, t, response);
                        Log.e("MainActivity", "" + t.toString());
                    }
                });
                Log.e("MainActivity", "onStart");
            }
        });
    }

    private void initClient() {
        RtnsSocketConfProvider confProvider = new RtnsSocketConfProvider("7e8224ffaec64a2dac57b5d3e25f3953",
                "007eJxTYAgUu7zOMPjjVsZz/H4zgqyYeHVvzWUuUbwR+mZTkU2djL0Cg3mqhZGRSVpaYmqymUmiUUpisql5kmmKcaqRaZqxpalxiPbNBIc6VoaKV4tYmRkYwRDEB+pMMTcyNjNNTbK0MDaxMDW2NE81TjVOs0wxMTNISklJ5GIwsrAwMjYxNDI3YILrQxZlYWBgAACziCll") {
            @Override
            public int getChainIdByAddress(SocketAddress address) {
                int chainId = 0;
                if (address instanceof InetSocketAddress) {
                    InetSocketAddress isa = (InetSocketAddress) address;
                    if ("121.196.198.83".equals(isa.getAddress().getHostAddress())) {
                        chainId = 336;
                    } else if ("echo.websocket.org".equals(isa.getAddress().getHostName())) {
                        chainId = 334;
                    } else if ("gateway.netless.link".equals(isa.getAddress().getHostName())) {
                        chainId = 370;
                    }
                }
                return chainId;
            }
        };
        client = new OkHttpClient.Builder()
                .socketFactory(new RtnsSocketFactory(confProvider))
                .retryOnConnectionFailure(true)
                .build();
    }

    private void output(final String txt) {
        runOnUiThread(() -> tv.append("\n\n" + txt));
    }
}