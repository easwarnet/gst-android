package com.tarjet.rtspapp;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Button;

import com.tarjet.rtspapp.R;

public class MainActivity extends AppCompatActivity {
    Button btRtspServer;
    Button btRtspClient;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        btRtspServer = findViewById(R.id.btRtspServer);
        btRtspClient = findViewById(R.id.btRtspClient);
        btRtspClient.setOnClickListener(view -> {
            Intent in = new Intent(getApplicationContext(), RtspClientConfigActivity.class);
            in.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
            startActivity(in);
        });
    }
}