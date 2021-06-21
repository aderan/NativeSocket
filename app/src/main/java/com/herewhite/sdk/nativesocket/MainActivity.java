package com.herewhite.sdk.nativesocket;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.IOException;

import androidx.appcompat.app.AppCompatActivity;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.WebSocket;
import okhttp3.WebSocketListener;
import okio.Buffer;
import okio.ByteString;

public class MainActivity extends AppCompatActivity {
    OkHttpClient client = new OkHttpClient.Builder()
            .socketFactory(new RtnsSocketFactory())
            .retryOnConnectionFailure(true)
            .build();

    TextView tv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = findViewById(R.id.sample_text);
        tv.setMovementMethod(ScrollingMovementMethod.getInstance());

        findViewById(R.id.test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // tv.setText(new NativeSocketHelper().stringFromJNI());
                testWebSocket();
                // testHttpGreeting();
            }

            private void testWebSocket() {
//                Request request = new Request.Builder().url("ws://121.196.198.83/echo").build();
                Request request = new Request.Builder().url("ws://121.196.198.83/echo").build();
//                Request request = new Request.Builder().url("wss://echo.websocket.org").build();
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
                        Log.e("NativeSocket", "" + t.toString());
                    }
                });
                Log.e("MainActivity", "onStart");
            }

            private void testHttpGreeting() {
                RtnsSocketImpl socketImpl = new RtnsSocketImpl(new NativeSocketHelper());
                try {
                    socketImpl.create(true);
                    socketImpl.connect("", 80);
                    Buffer buffer = new Buffer();
                    buffer.writeUtf8("GET /greeting HTTP/1.1\r\n")
                            .writeUtf8("Host: 127.0.0.1\r\n")
                            .writeUtf8("Connection: Close\r\n\r\n");
                    socketImpl.getOutputStream().write(buffer.readByteArray());

                    byte[] data = new byte[65520];
                    int len = 0;
                    while (len < 10) {
                        int size = socketImpl.getInputStream().read(data, len, data.length - len);
                        if (size > 0) {
                            len += size;
                        } else {
                            break;
                        }
                    }
                    byte[] result = new byte[len];
                    System.arraycopy(data, 0, result, 0, len);
                    Log.e("NativeSocket", new String(result));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    private void output(final String txt) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                tv.append("\n\n" + txt);
            }
        });
    }
}