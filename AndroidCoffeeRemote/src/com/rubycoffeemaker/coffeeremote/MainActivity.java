package com.rubycoffeemaker.coffeeremote;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.concurrent.ExecutionException;

import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.StatusLine;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.ToggleButton;

public class MainActivity extends Activity {
	String start_ext  = "/make_coffee";
	String stop_ext = "/stop_coffee";
	String status_ext = "/status";
	
	String brewing_string = "Coffee is brewing";
	String confirm_start = "Your coffee will be brewed soon.";
	String confirm_stop = "The coffee maker will be stopped.";
	
	boolean brewing = false;
	
	ProgressBar mySpinner;
	ToggleButton coffeeToggleButton;
	TextView statusText;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		mySpinner = (ProgressBar) findViewById(R.id.spinner);
		coffeeToggleButton = (ToggleButton) findViewById(R.id.coffeeToggleButton);
		statusText = (TextView) findViewById(R.id.statusText);
		
		setInitial();
	}

	public void setInitial(){
		try {
			if (checkConnection()){
				brewing = isBrewing();
				coffeeToggleButton.setChecked(brewing);
			} else {
				statusText.setText("No Connection");
			}
		} catch (InterruptedException e) {
			statusText.setText("Interrupted Exception");
			e.printStackTrace();
		} catch (ExecutionException e) {
			statusText.setText("Execution Exception");
			e.printStackTrace();
		}
	}
	
	public void sendRequest(View view) {		
		mySpinner.setVisibility(View.VISIBLE);
		coffeeToggleButton.setEnabled(false);
		
		try {
			NetWorker checker = new NetWorker();
			AsyncTask<String, Void, String> result;
			
			if (brewing) {
				result = checker.execute(stop_ext);
				brewing = false;
			} else {
				result = checker.execute(start_ext);
				brewing = true;
			}
			
			result.get();
		} catch (InterruptedException e) {
			statusText.setText("Interrupted Exception");
			e.printStackTrace();
		} catch (ExecutionException e) {
			statusText.setText("Execution Exception");
			e.printStackTrace();
		}
			
		mySpinner.setVisibility(View.INVISIBLE);
		coffeeToggleButton.setEnabled(true);
	}
	
	private boolean checkConnection() throws InterruptedException, ExecutionException {
		NetWorker checker = new NetWorker();
		AsyncTask<String, Void, String> result = checker.execute(status_ext);
		
		String response_string = result.get();
		
		if (!response_string.equals("")) {
			return true;
		} else {
			return false;
		}
	}
	
	private boolean isBrewing() throws InterruptedException, ExecutionException {
		NetWorker checker = new NetWorker();
		AsyncTask<String, Void, String> result = checker.execute(status_ext);
		
		String response_string = result.get();
		
		if (response_string.equals(brewing_string)) {
			return true;
		} else {
			return false;
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

}

class NetWorker extends AsyncTask<String, Void, String> {
	
	@Override
	protected String doInBackground(String... ext) {
		String response_string = "";
		String base_URL = "ruby-coffee-maker.herokuapp.com";
		
		HttpClient client = new DefaultHttpClient();
		
		try {
			URI get_URI = new URI("http", base_URL, ext[0], null);
			
			HttpResponse response = client.execute(new HttpGet(get_URI.toString()));
		
			StatusLine statusLine = response.getStatusLine();		
			
			if (statusLine.getStatusCode() == HttpStatus.SC_OK) {
		        ByteArrayOutputStream out = new ByteArrayOutputStream();
		        response.getEntity().writeTo(out);
		        out.close();
		        
		        response_string = out.toString();
			}
		} catch (ClientProtocolException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (URISyntaxException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return response_string;
	}
	
}

