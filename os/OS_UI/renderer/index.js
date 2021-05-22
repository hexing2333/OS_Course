var stop_flag = 0;
var equip = document.querySelector("#customers"); //设备管理器dom
var proc_state = document.querySelector("#state"); //进程执行状态dom
var proc_state_out = document.querySelector("#proc_state_out"); //进程执行状态外框dom
var proc_list = document.querySelector("#process_list"); //进程执行列表dom


function system_start() {
    os_start()
    // //2.更新设备管理器
    // equip.innerHTML = `<tr>
    //                         <th>设备名称</th>
    //                         <th>进程状态</th>
    //                         <th>占用进程</th>
    //                     </tr>
    //                     <tr>
    //                         <td>打印机</td>
    //                         <td>进程0</td>
    //                         <td>进程1</td>
    //                     </tr>
    //                     <tr>
    //                         <td>显示器</td>
    //                         <td>进程2</td>
    //                         <td>进程4</td>
    //                     </tr>
    //                     <tr>
    //                         <td>U盘</td>
    //                         <td>进程6</td>
    //                         <td>进程3</td>
    //                     </tr>
    //                     <tr>
    //                         <td>鼠标</td>
    //                         <td>进程4</td>
    //                         <td>进程2</td>
    //                     </tr>
    //                     <tr>
    //                         <td>键盘</td>
    //                         <td>进程9</td>
    //                         <td>进程0</td>
    //                     </tr>`;

    //3.启动内存与磁盘监控
    stop_flag = 0;
    document.getElementById('start').className = 'layui-btn layui-btn-lg layui-btn-radius layui-btn-disabled';
    document.getElementById('stop').className = 'layui-btn layui-btn-lg layui-btn-radius layui-btn-warm';
    // layui.use('element', function() {
    //     var $ = layui.jquery,
    //         element = layui.element; //Tab的切换功能，切换事件监听等，需要依赖element模块
    //     element.progress('memory_usage', '0%');
    //     element.progress('disk_usage', '0%');
    //     timer1 = setInterval(function() {
    //         element.progress('memory_usage', Math.floor(Math.random() * 100) + '%');
    //         element.progress('disk_usage', Math.floor(Math.random() * 100) + '%');
    //         if (stop_flag == 1) {
    //             element.progress('memory_usage', '0%');
    //             element.progress('disk_usage', '0%');
    //             clearInterval(timer1)
    //         }
    //     }, 1000);
    // });

    // var main_clock = document.getElementById("main_clock");
    // var time_count = 0;
    // timer2 = setInterval(function() {
    //     main_clock.innerHTML = "主控时钟: " + time_count;
    //     time_count += 1;
    //     if (stop_flag == 1) {
    //         main_clock.innerHTML = "主控时钟: 0 ";
    //         clearInterval(timer2)
    //     }
    // }, 1000);

    // //更新进程执行状态
    // var ps = `<h3>进程1：NOP</h3>`
    // var ps0 = `<h3>进程1：NOP<h3>`
    // var ps_count = 0;
    // timer3 = setInterval(function() {
    //     proc_state.innerHTML = ps;
    //     ps = ps + ps0;
    //     ps_count = ps_count + 1;
    //     if (stop_flag == 1) {
    //         proc_state.innerHTML = "";
    //         clearInterval(timer3)
    //     }
    // }, 1000);

    
};

function system_stop() {
    stop_flag = 1;
    os_stop()
}