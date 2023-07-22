package com.tarjet.rtspapp;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

public class RtspServer extends AppCompatActivity {
    // Initialize native code, build pipeline, etc
    private native void nativeInit(boolean credentialsEnabled, String username, String password);
    private native void nativeFinalize(); // Destroy pipeline and shutdown native code
    private native void nativeStart();     // Set pipeline to PLAYING
    private static native boolean nativeClassInit(); // Initialize native class: cache Method IDs for callbacks

    private long native_custom_data;      // Native code will use this to keep private data
    Button btStartRtspServer;
    Button btStopRtspServer;
    TextView tvRtspServerStatus = findViewById(R.id.tvRtspServerStatus);
    static {
        System.loadLibrary("gstreamer_android");
        System.loadLibrary("rtspserver");
        nativeClassInit();
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rtsp_server);
        btStartRtspServer = findViewById(R.id.btStartRtspServer);
        btStopRtspServer = findViewById(R.id.btStopRtspServer);

        btStartRtspServer.setOnClickListener(view -> {
            Toast.makeText(this, "IP Address: " + getIpAddress(), Toast.LENGTH_SHORT).show();
            nativeInit(false, "", "");
            nativeStart();
        });
    }

    // Called from native code. This sets the content of the TextView from the UI thread.
    private void setMessage(final String message) {

//        runOnUiThread (new Runnable() {
//            public void run() {
//                tvRtspServerStatus.setText(message);
//            }
//        });
    }

    // Called from native code. Native code calls this once it has created its pipeline and
    // the main loop is running, so it is ready to accept commands.
    private void onGStreamerInitialized () {
        tvRtspServerStatus.setText("GStreamer Initialized");
    }

    private String getIpAddress() {
        String ip = "";
        try {
            Enumeration<NetworkInterface> enumNetworkInterfaces = NetworkInterface
                    .getNetworkInterfaces();
            while (enumNetworkInterfaces.hasMoreElements()) {
                NetworkInterface networkInterface = enumNetworkInterfaces
                        .nextElement();
                Enumeration<InetAddress> enumInetAddress = networkInterface
                        .getInetAddresses();
                while (enumInetAddress.hasMoreElements()) {
                    InetAddress inetAddress = enumInetAddress.nextElement();

                    if (inetAddress.isSiteLocalAddress()) {
                        ip += inetAddress.getHostAddress();
                    }

                }

            }

        } catch (SocketException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            ip += "Something Wrong! " + e.toString() + "\n";
        }

        return ip;
    }

}