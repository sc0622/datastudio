
const {app, BrowserWindow, ipcMain, ipcRenderer, Menu } = require('electron');
const path = require('path')
const url = require('url')

let win = null;

function load(win, local) {
    if (local) {
        win.loadURL('http://localhost:4200/')
    } else {
        win.loadURL(url.format({
        pathname: path.join(__dirname, 'dist', 'datastudio', 'index.html'),
            protocol: 'file:',
            slashes: true
        }))
    }
}

function createWindow() {
    // Create the browser window
    win = new BrowserWindow({
        title: 'Data Studio',
        width: 1000,
        height: 600,
        backgroundColor: '#ffffff',
        slashes: true,
        icon: `file://${__dirname}/dist/favicon.ico`
    })
 
    //
    load(win, true);
    //
    win.webContents.openDevTools()

    win.webContents.on('crashed', function () {
      const options = {
        type: 'info',
        title: '渲染器进程崩溃',
        message: '这个进程已经崩溃.',
        buttons: ['重载', '关闭']
      }
      dialog.showMessageBox(options, function (index) {
        if (index === 0) win.reload()
        else win.close()
      })
      app.quit();
    })

    //
    win.on('close', () => {
        win = null;
        app.quit();
    })

    // create menu
    const menu = Menu.buildFromTemplate(template)
    Menu.setApplicationMenu(menu)

    //
    
    //TEST
    let protocore;
    if (process.env.ELECTRON_ENV === 'dev') {
        protocore = require('./lib/protocore/protocore.node');
    } else {
        protocore = require(`${__dirname}/dist/datastudio/protocore.node`);
    }
    var vehicle = new protocore.Vehicle();
    console.log(vehicle.mark);
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

let template = [{
  label: '编辑',
  submenu: [{
    label: '撤销',
    accelerator: 'CmdOrCtrl+Z',
    role: 'undo'
  }, {
    label: '重做',
    accelerator: 'Shift+CmdOrCtrl+Z',
    role: 'redo'
  }, {
    type: 'separator'
  }, {
    label: '剪切',
    accelerator: 'CmdOrCtrl+X',
    role: 'cut'
  }, {
    label: '复制',
    accelerator: 'CmdOrCtrl+C',
    role: 'copy'
  }, {
    label: '粘贴',
    accelerator: 'CmdOrCtrl+V',
    role: 'paste'
  }, {
    label: '全选',
    accelerator: 'CmdOrCtrl+A',
    role: 'selectall'
  }]
}];
