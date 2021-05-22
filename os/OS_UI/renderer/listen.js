var http = require('http');
var fs = require('fs');
var exec = require('child_process').exec;
var url = require('url');
//此为渲染进程，需从主进程获得BrowserWindow模块
const BrowserWindow = require('electron').remote.BrowserWindow;
const { SSL_OP_EPHEMERAL_RSA } = require('constants');

let memory = '';
let disk = '';
let process = ''
let state = ''
let tree = ''
let device = ''
let frame = ''
let disk_block = ''
let timerid = 0
let pc = ''

function os_start() {
    rdfile()
    timerid = setInterval(() => {
        rdfile()
        process_update()
        state_update()
        root_update()
        device_update()
        frame_update()
        disk_update()
        pc_update()
        al_update()
    }, 100);
}



function os_stop() {
    clearInterval(timerid);//停止
    document.getElementById('start').className = 'layui-btn layui-btn-lg layui-btn-radius layui-btn-normal';
    document.getElementById('stop').className = 'layui-btn layui-btn-lg layui-btn-radius layui-btn-disabled';
    treein = document.querySelector("#tree") //进程执行状态dom
    equip.innerHTML = `<tr><th>设备名称</th><th>设备状态</th><th>占用进程</th></tr>`;
    proc_list.innerHTML = `<tr><th>进程ID</th><th>运行状态</th><th>内存</th><th>优先级</th><th>调度策略</th></tr>`;
    var memory_block = document.querySelector("#memory_block"); //内存块dom
    memory_block.innerHTML = `
    <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>
                            <div style="background-color:rgb(145, 241, 241);width:3%"></div>`
    treein.innerHTML = ``
    var disk_block = document.querySelector("#disk_block"); //内存块dom
    disk_block.innerHTML = `<div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>
    <div style="background-color:rgb(247, 209, 152);width:3%"></div>`

    layui.use('element', function () {
        var $ = layui.jquery,
            element = layui.element; //Tab的切换功能，切换事件监听等，需要依赖element模块
        element.progress('memory_usage', '0%');
        element.progress('disk_usage', '0%');
    });

    let ps = document.querySelector("#state") //进程执行状态dom
    ps.innerHTML = ``
}


function al_update(){
    var  sel=document.getElementById("al_select");
    fs.writeFile('./commute/al.txt', sel.value, function(err) {
        if (err) {
            throw err;
        }
    });
}

function process_update() {
    let temparr = process.split("\n")
    let updatestr = '';
    for (let i = 0; i < temparr.length; i++) {
        let arr = temparr[i].split(" ")
        //alert("进程ID:" + arr[0] + "进程状态" + arr[1] + "优先级" + arr[2])
        updatestr = updatestr +
            `<tr>
                <td>`+ arr[0] + `</td>
                <td>`+ arr[1] + `</td>
                <td>`+ arr[2] + `</td>
            </tr>`
    }
    var proc_list = document.querySelector("#process_list"); //进程执行列表dom
    proc_list.innerHTML = `<tr>
    <th>进程ID</th>
    <th>运行状态</th>
    <th>优先级</th>
    </tr>`+ updatestr
}

function disk_update() {
    let temparr = disk_block.split("\n")
    let updatestr = '';

    total_usage = temparr[0].split(" ")
    total = total_usage[0]
    usage = total_usage[1]
    width = parseInt(100 / total)

    for (let i = 1; i < temparr.length - 1; i++) {
        let aaa = temparr[i].split(" ")
        if (aaa[1] == -1) {
            updatestr = updatestr + `<div style="background-color:rgb(247, 209, 152);width:` + width + `%"></div>`
        }
        else {
            updatestr = updatestr + `<div style="background-color:rgb(255, 153, 0);width:` + width + `%"></div>`
        }
    }

    var proc_list = document.querySelector("#disk_block"); //内存块dom
    proc_list.innerHTML = updatestr

    layui.use('element', function () {
        var $ = layui.jquery,
            element = layui.element; //Tab的切换功能，切换事件监听等，需要依赖element模块
        element.progress('disk_usage', usage);
    });
}

function device_update() {
    let temparr = device.split("\n")
    let updatestr = '';
    for (let i = 0; i < temparr.length - 1; i++) {
        let arr = temparr[i].split(" ")
        updatestr = updatestr +
            `<tr>
                <td>`+ arr[0] + `</td>
                <td>`+ arr[1] + `</td>
                <td>`+ arr[2] + `</td>
            </tr>`
    }
    var equip1 = document.querySelector("#customers"); //设备管理器dom
    equip1.innerHTML = `<tr>
    <th>设备名称</th>
    <th>设备状态</th>
    <th>占用进程</th>
    </tr>`+ updatestr
}

function state_update() {
    let temparr = state.split("\n")
    let updatestr = '';
    for (let i = 0; i < temparr.length; i++) {
        updatestr = updatestr + `<h3>` + temparr[i] + `<h3>`
    }
    let ps = document.querySelector("#state") //进程执行状态dom
    ps.innerHTML = updatestr
}

function pc_update() {
    let zhukongdom = document.querySelector("#main_clock") //进程执行状态dom
    zhukong = '主控时钟: ' + pc
    zhukongdom.innerHTML = zhukong
}

function root_update() {
    let temparr = tree.split("\n")
    let updatestr = '';
    for (let i = 0; i < temparr.length; i++) {
        updatestr = updatestr + `<h3>` + temparr[i] + `<h3>`
    }
    let treein = document.querySelector("#tree") //进程执行状态dom
    treein.innerHTML = updatestr
}

function process_update() {
    let temparr = process.split("\n")
    let updatestr = '';
    for (let i = 0; i < temparr.length - 1; i++) {
        let arr = temparr[i].split(" ")
        //alert("进程ID:" + arr[0] + "进程状态" + arr[1] + "优先级" + arr[2])
        updatestr = updatestr +
            `<tr>
                <td>`+ arr[0] + `</td>
                <td>`+ arr[1] + `</td>
                <td>`+ arr[2] + `</td>
                <td>`+ arr[3] + `</td>
                <td>`+ arr[4] + `</td>
            </tr>`
    }
    var proc_list = document.querySelector("#process_list"); //进程执行列表dom
    proc_list.innerHTML = `<tr>
    <th>进程ID</th>
    <th>运行状态</th>
    <th>内存</th>
    <th>优先级</th>
    <th>调度策略</th>
    </tr>`+ updatestr
}

function frame_detail(detail){
    alert("页号："+detail)
}

function frame_update() {
    let temparr = frame.split("\n")
    let updatestr = '';

    total_usage = temparr[0].split(" ")
    total = total_usage[0]
    usage = total_usage[1]
    width = parseInt(100 / total)

    for (let i = 1; i < temparr.length - 1; i++) {
        let aaa = temparr[i].split(" ")
        if (aaa[1] == -1) {
            updatestr = updatestr + `<div style="background-color:rgb(145, 241, 241);width:` + width + `%"></div>`
        }
        else {
            updatestr = updatestr + `<div style="background-color:rgb(24, 182, 182);width:` + width + `%" onclick="frame_detail(`+aaa[2]+`)">` + aaa[1] + `</div>`
        }
    }

    var proc_list = document.querySelector("#memory_block"); //内存块dom
    proc_list.innerHTML = updatestr

    layui.use('element', function () {
        var $ = layui.jquery,
            element = layui.element; //Tab的切换功能，切换事件监听等，需要依赖element模块
        element.progress('memory_usage', usage);
    });
}

function rdfile() {

    // 内存读取
    fs.readFile('./commute/memory.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        memory = data.toString()
    })
    // 磁盘读取
    fs.readFile('./commute/disk.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        disk = data.toString()
    })
    // 进程列表读取
    fs.readFile('./commute/process.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        process = data.toString()
    })
    // 进程状态读取
    fs.readFile('./commute/state.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        state = data.toString()
    })
    // 目录树读取
    fs.readFile('./commute/tree.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        tree = data.toString()
    })
    // 设备读取
    fs.readFile('./commute/device.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        device = data.toString()
    })

    // 内存块读取
    fs.readFile('./commute/frame.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        frame = data.toString()
    })

    // 磁盘块读取
    fs.readFile('./commute/storageStatus.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        disk_block = data.toString()
    })

    // 磁盘块读取
    fs.readFile('./commute/pc.txt', function (err, data) {
        if (err) {
            return alert(err);
        }
        pc = data.toString()
    })
}




// 创建服务器
http.createServer(function (request, response) {
    // 解析请求，包括文件名
    var pathname = url.parse(request.url).pathname;
    arr = pathname.split("/");
    if (arr[1] == 'memory_usage') {
        layui.use('element', function () {
            var $ = layui.jquery,
                element = layui.element; //Tab的切换功能，切换事件监听等，需要依赖element模块
            element.progress('memory_usage', arr[2]);
        });
    } else if (arr[1] == 'disk_usage') {
        layui.use('element', function () {
            var $ = layui.jquery,
                element = layui.element; //Tab的切换功能，切换事件监听等，需要依赖element模块
            element.progress('disk_usage', arr[2]);
        });
    } else if (arr[1] == 'equip_manager') //设备管理器请求
    {

    } else if (arr[1] == 'state') //进程执行状态请求
    {

    } else if (arr[1] == 'list') //进程列表请求
    {

    }
}).listen(3579);


function execute(cmd) {

    exec(cmd, function (error, stdout, stderr) {
        if (error) {
            console.error(error);
        }
        else {
            console.log("success");
        }
    });
}

// 控制台会输出以下信息
console.log('Server running at http://127.0.0.1:3579/');

function add_device() {
    execute('cd commute & add_device.txt')
}


function deleted_device() {
    execute('cd commute & delete_device.txt')
}