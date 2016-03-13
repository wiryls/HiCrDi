using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace WPFGUI.Other
{
    class ActionHelper
    {
        public void Jump()
        {
            if (!isSpacePressed)
                KeyboardSimulator.Press(Key.Space);
            else
                KeyboardSimulator.Release(Key.Space);

            isSpacePressed = !isSpacePressed;
        }

        private bool isSpacePressed = false;
    }


    /* References：
     * 
     * √[.net中模拟键盘和鼠标操作]
     * (http://www.cnblogs.com/sixty/archive/2009/08/09/1542210.html)
     * ?[Windows Input Simulator (C# SendInput Wrapper - Simulate Keyboard and Mouse)]
     * (https://inputsimulator.codeplex.com/)
     * ×[WPF – Send keys]
     * (https://michlg.wordpress.com/2013/02/05/wpf-send-keys/)
     */
}
