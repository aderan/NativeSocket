package com.herewhite.sdk.nativesocket;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import okhttp3.OkHttpClient;

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
                tv.setText(new NativeSocketHelper().stringFromJNI());

//                Request request = new Request.Builder().url("ws://121.196.198.83/site/change").build();
//                WebSocket ws = client.newWebSocket(request, new WebSocketListener() {
//                    @Override
//                    public void onOpen(WebSocket webSocket, Response response) {
//                        webSocket.send("Hello, it's SSaurel !");
//                        webSocket.send("What's up ?");
//                        webSocket.send(ByteString.decodeHex("deadbeef"));
//
//                        webSocket.close(1000, "Goodbye !");
//                    }
//
//                    @Override
//                    public void onMessage(WebSocket webSocket, String text) {
//                        output(text);
//                    }
//
//                    @Override
//                    public void onMessage(WebSocket webSocket, ByteString bytes) {
//                        output(bytes.toString());
//                    }
//
//                    @Override
//                    public void onClosing(WebSocket webSocket, int code, String reason) {
//                        super.onClosing(webSocket, code, reason);
//                        webSocket.close(1000, null);
//                        output("Closing : " + code + " / " + reason);
//                    }
//
//                    @Override
//                    public void onClosed(WebSocket webSocket, int code, String reason) {
//                        super.onClosed(webSocket, code, reason);
//                        output("onClosed : " + code + " / " + reason);
//                    }
//
//                    @Override
//                    public void onFailure(WebSocket webSocket, Throwable t, Response response) {
//                        super.onFailure(webSocket, t, response);
//                    }
//                });
//                  client.dispatcher().executorService().shutdown();

//                RtnsSocketImpl socketImpl = new RtnsSocketImpl(socket);
//                try {
//                    socketImpl.create(true);
//                    socketImpl.connect("", 0);
//                    socketImpl.getOutputStream().write('a');
//
//                    byte[] data = new byte[65520];
//                    int len = 0;
//                    while (len < data.length) {
//                        int size = socketImpl.getInputStream().read(data, len, data.length - len);
//                        if (size > 0) {
//                            len += size;
//                        }
//                    }
//                    Log.e("NativeSocket", "data " + new String(data));
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
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