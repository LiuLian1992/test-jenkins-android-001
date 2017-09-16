package com.hello.demo;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends Activity {

        private TextView text;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
                text = (TextView) findViewById(R.id.text);
	}

        public void addBigInt(View v) {
                byte[] result = NativeAuthenticatorKernel.bigIntAdd();
                StringBuilder res = new StringBuilder();
                Log.d("uTClient", result.length + "");
                for(byte b : result)
                {
                    res.append(Integer.toHexString(b & 0xFF));
                }
                text.setText(res.toString());
	}
}
