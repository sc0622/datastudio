
const {app, BrowserWindow, ipcMain, ipcRenderer, Menu} = require('electron');
const path = require('path')
const url = require('url')

let projectDir = path.resolve('../..');
if (process.env.NODE_ENV) {
  process.env.Path += ';' + path.resolve(process.env.QTDIR_64, 'bin') + ';' +
      path.resolve(projectDir, 'lib/3rdpart/moxa');
} else {
  process.env.Path += ';' +
      `${__dirname}`;
}

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
    slashes: true
  })

  if (process.env.NODE_ENV) {
    win.setIcon('./src/favicon.ico');
  }
  else {
    win.setIcon(`file://${__dirname}/dist/favicon.ico`);
  }

  //
  load(win, true);
  //
  win.webContents.openDevTools()

  win.webContents.on('crashed', function() {
    const options = {
      type: 'info',
      title: '渲染器进程崩溃',
      message: '这个进程已经崩溃.',
      buttons: ['重载', '关闭']
    };
    dialog.showMessageBox(options, function(index) {
      if (index === 0)
        win.reload()
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

  // TEST
  let jx;
  if (process.env.NODE_ENV) {
    jx = require('./lib/protocore/protocore.node');
  } else {
    jx = require(`${__dirname}/dist/datastudio/protocore.node`);
  }
  var vehicle = new jx.Vehicle();
  console.log(vehicle.mark);

  let config = {
    sourceType: 'file',
    filePath: path.resolve(projectDir, 'config/protocol.json')
  };
  var parser = jx.Parser.create(JSON.stringify(config));
  if (parser) {
    var object = parser.parse(
        '1001/2001/ICDTable_0ceb8821fc5d4990bd35faa5a29f6f79', 3, 1);
    var a = object.name;
    console.log(object.name);
  }
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
  submenu: [
    {label: '撤销', accelerator: 'CmdOrCtrl+Z', role: 'undo'},
    {label: '重做', accelerator: 'Shift+CmdOrCtrl+Z', role: 'redo'},
    {type: 'separator'},
    {label: '剪切', accelerator: 'CmdOrCtrl+X', role: 'cut'},
    {label: '复制', accelerator: 'CmdOrCtrl+C', role: 'copy'},
    {label: '粘贴', accelerator: 'CmdOrCtrl+V', role: 'paste'},
    {label: '全选', accelerator: 'CmdOrCtrl+A', role: 'selectall'}
  ]
}];
