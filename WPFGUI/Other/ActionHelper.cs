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
            if (isDownPressed) {
                KeyboardSimulator.Release(Key.Down);
                isDownPressed = false;
            }

            if (!isSpacePressed) {
                KeyboardSimulator.Press(Key.Space);
                isSpacePressed = true;
            }
        }

        public void Down()
        {
            if(isSpacePressed) {
                KeyboardSimulator.Release(Key.Space);
                isSpacePressed = false;
            }

            if(!isDownPressed) {
                KeyboardSimulator.Press(Key.Down);
                isDownPressed = true;
            }
        }

        public void Idel()
        {
            if(isDownPressed) {
                KeyboardSimulator.Release(Key.Down);
                isDownPressed = false;
            }

            if(isSpacePressed) {
                KeyboardSimulator.Release(Key.Space);
                isSpacePressed = false;
            }
        }

        private bool isSpacePressed = false;
        private bool isDownPressed = false;
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
