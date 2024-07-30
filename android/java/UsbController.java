// File: UsbController.java
package org.qtproject.example;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.Log;

import java.util.HashMap;

public class UsbController {
    private Context context;
    private UsbManager usbManager;
    private PendingIntent permissionIntent;

    public UsbController(Context context) {
        this.context = context;
        this.usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        this.permissionIntent = PendingIntent.getBroadcast(context, 0, new Intent("org.qtproject.example.USB_PERMISSION"), 0);
    }

    public String[] getDeviceList() {
        HashMap<String, UsbDevice> usbDevices = usbManager.getDeviceList();
        String[] deviceNames = new String[usbDevices.size()];
        int index = 0;
        for (UsbDevice device : usbDevices.values()) {
            deviceNames[index++] = device.getDeviceName();
            // Optionally, request permission
            usbManager.requestPermission(device, permissionIntent);
        }
        return deviceNames;
    }

    // Additional methods for open, close, read, and write operations
}
