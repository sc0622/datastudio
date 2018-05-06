
const {app, BrowserWindow, ipcMain, ipcRenderer } = require('electron');
const path = require('path')
const url = require('url')

//TEST
const protocore = require('../lib/release/protocore.node');
var vehicle = new protocore.Vehicle();

let win = null;

function createWindow() {
    // Create the browser window
    win = new BrowserWindow({
        width: 1000,
        height: 600,
        backgroundColor: '#ffffff',
        icon: `file://${__dirname}/dist/favicon.ico`
    })
 
    //
    /*
    win.loadURL(url.format({
        pathname: path.join(__dirname, 'dist', 'index.html'),
        protocol: 'file:',
        slashes: true
      }))*/
    win.loadURL('http://localhost:4200/')

    //
    //win.webContents.openDevTools()

    //
    win.on('close', () => {
        win = null;
    })
}

//
app.on('ready', createWindow);

app.on('all-window-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})

app.on('active', () => {
    if (win === null) {
        createWindow()
    }
})
