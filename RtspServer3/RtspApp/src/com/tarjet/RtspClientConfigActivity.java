package com.tarjet;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

public class RtspClientConfigActivity extends AppCompatActivity {
    Button btStartRtspClient;
    EditText etvUri;
    EditText etvUsername;
    EditText etvPassword;
    Switch switchLogin;
    TextView tvUsername;
    TextView tvPassword;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rtsp_client_config);
        btStartRtspClient = findViewById(R.id.btStartRtspClient);
        etvUri = findViewById(R.id.etvUri);
        switchLogin = findViewById(R.id.switchLogin);
        etvUsername = findViewById(R.id.etvUsername);
        etvPassword = findViewById(R.id.etvPassword);
        tvUsername = findViewById(R.id.tvUsername);
        tvPassword = findViewById(R.id.tvPassword);

        switchLogin.setOnClickListener(view -> {
            if(switchLogin.isChecked()) {
                etvUsername.setVisibility(TextView.VISIBLE);
                etvPassword.setVisibility(TextView.VISIBLE);
                tvUsername.setVisibility(TextView.VISIBLE);
                tvPassword.setVisibility(TextView.VISIBLE);
            } else {
                etvUsername.setVisibility(TextView.GONE);
                etvPassword.setVisibility(TextView.GONE);
                tvUsername.setVisibility(TextView.GONE);
                tvPassword.setVisibility(TextView.GONE);
            }
        });

        btStartRtspClient.setOnClickListener(view -> {
            Intent in = new Intent(RtspClientConfigActivity.this, RtspClient.class);
            //Bundle extras = new Bundle();
            //extras.putString("Uri", etvUri.getText().toString());
            //extras.putString("Username", etvUri.getText().toString());
            //extras.putString("Password", etvUri.getText().toString());
            //Toast.makeText(this, etvUri.getText().toString(), Toast.LENGTH_SHORT).show();
            in.putExtra("Uri", etvUri.getText().toString());
            in.putExtra("Username", etvUsername.getText().toString());
            in.putExtra("Password", etvPassword.getText().toString());
            startActivity(in);
        });
    }
}