# Data Directory

This directory contains all the static files that will be served by the ESP32 web server. Files placed in this directory are uploaded to LittleFS and made accessible via HTTP.

## Overview

The `data` directory is where you store:
- HTML files (like `index.html`)
- CSS stylesheets
- JavaScript files
- Images and other static assets
- Any other files you want to serve via the web server

## File Serving

Files in this directory are served at the root path of your web server. For example:

- `data/index.html` → accessible at `http://[ESP32_IP]/index.html` or `http://[ESP32_IP]/`
- `data/style.css` → accessible at `http://[ESP32_IP]/style.css`
- `data/app.js` → accessible at `http://[ESP32_IP]/app.js`
- `data/images/logo.png` → accessible at `http://[ESP32_IP]/images/logo.png`

## Uploading Files

To upload files from this directory to your ESP32:

1. **Arduino IDE**: Use **Tools → ESP32 Sketch Data Upload** to upload all files in this directory to LittleFS
2. **PlatformIO**: Use the filesystem upload feature in PlatformIO

**Important**: Files must be uploaded to LittleFS before they can be served by the web server. Simply placing files in this directory is not enough - they need to be uploaded to the ESP32's flash memory.

## Current Files

- `index.html` - The main HTML page served at the root path

## Adding New Files

To add new files:

1. Place the file in this `data` directory (or a subdirectory)
2. Upload the files to LittleFS using Arduino IDE or PlatformIO
3. Access the file via `http://[ESP32_IP]/filename`

## File Structure Example

```
data/
├── index.html          # Main page
├── style.css          # Stylesheet
├── app.js             # JavaScript
└── images/            # Image assets
    ├── logo.png
    └── icon.ico
```

All files maintain their directory structure when uploaded to LittleFS.

## Notes

- File names are case-sensitive
- The web server automatically serves `index.html` for root path requests (`/`)
- If a requested file doesn't exist, the server will try to serve `index.html` as a fallback
- File size is limited by available flash memory on your ESP32

