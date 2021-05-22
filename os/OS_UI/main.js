var electron = require('electron')

var app = electron.app

var BrowserWindow = electron.BrowserWindow;

var mainWindow = null;
app.on('ready', () => {
    const Menu = electron.Menu
    //Menu.setApplicationMenu(null)  //隐藏菜单栏
    mainWindow = new BrowserWindow({
        width: 1080,
        height: 720,
        titleBarStyle: 'customButtonsOnHover',
        frame: true,
        transparent: false,
        vibrancy: 'popover',
        webPreferences: {
            nodeIntegration: true, // 是否集成 Nodejs
            enableRemoteModule: true, //这句必须要有,否则 require("electron").remote.BrowserWindow;是空
            contextIsolation: false
        }
    })

    mainWindow.loadFile('index.html')
})