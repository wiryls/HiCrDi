using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Timers;
using System.Windows;
using System.Windows.Media.Imaging;
using WPFGUI.Other;

namespace WPFGUI
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            mainTimer.Elapsed += new ElapsedEventHandler(ProcessFrame);
        }

        static int i = 0;
        private void ProcessFrame(object source, ElapsedEventArgs e)
        {
            i++;

            if(i % 7 == 0)
                action.Jump();
            
            var fs = File.Open("pics\\" + i.ToString() + ".png", FileMode.OpenOrCreate);
            System.Drawing.Bitmap bitmap = null;

            MainFrame.Dispatcher.Invoke( new Action (delegate {
                bitmap = ImageHelper.CaptureElement(MainFrame);
            }));

            var bitmapSource = ImageHelper.Bitmap2BitmapSource(bitmap);
            var img = new IntermediateImage(bitmapSource);
            ImageHelper.GenerateImage(bitmapSource, ".png", fs);
            fs.Close();
        }


        private ActionHelper action = new ActionHelper();

        private Timer mainTimer = new Timer() {
            Interval = 50,
            Enabled = false,
        };

        /*
         * References：
         * 
         * [Repeating a function every few seconds]
         * (http://stackoverflow.com/questions/11296897/repeating-a-function-every-few-seconds)
         */

    }
}
