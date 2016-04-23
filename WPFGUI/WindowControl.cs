using System;
using System.Timers;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using WPFGUI.Other;

namespace WPFGUI
{
    public partial class MainWindow : Window
    {
        static int i = 0;
        private void ProcessFrame(object source, ElapsedEventArgs e)
        {
            // TODO:
            if(i++ % 7 == 7)
                this.action.Jump();

            if(isTesting) {

                mutex.WaitOne();

                var bitmap = this.Dispatcher.Invoke(()=> {
                    return ImageHelper.CaptureElement(this.MainFrame);
                });

                BitmapSource bitmapSource = ImageHelper.Bitmap2BitmapSource(bitmap);

                // TODO: 
                var tmp = new IntermediateImage(bitmapSource);
                libheartbeat.test(tmp.ImagePixels, tmp.ImagePixels, (uint)tmp.Info.Width, (uint)tmp.Info.Height);
                bitmapSource = tmp.ToBitmapSource();

                if(isTesting) {
                    bitmapSource.Freeze();
                    this.Dispatcher.BeginInvoke(new Action<BitmapSource>((img) =>
                        this.DebugOutput.Source = img
                    ), bitmapSource);
                }

                mutex.ReleaseMutex();
            }

            //{
            //    var img = new IntermediateImage(bitmapSource);
            //    var fs = File.Open("pics\\" + i.ToString() + ".png", FileMode.OpenOrCreate);
            //    ImageHelper.GenerateImage(bitmapSource, ".png", fs);
            //    fs.Close();
            //}

            /* References:
             *
             * [Passing Objects as Arguments to Dispatcher.Invoke in WPF]
             * (http://stackoverflow.com/questions/24790835/passing-objects-as-arguments-to-dispatcher-invoke-in-wpf)
             * [Dispatch.Invoke( new Action…) with a parameter]
             * (http://stackoverflow.com/questions/14738533/dispatch-invoke-new-action-with-a-parameter)
             */
        }

        private void UpdateBorderEffect()
        {
            ColorAnimation animation = new ColorAnimation()
            {
                Duration = new Duration(TimeSpan.FromSeconds(0.618)),
                FillBehavior = FillBehavior.HoldEnd
            };

            if (!isRunning) {
                animation.To   = Color.FromScRgb(0.8f, 1.0f, 1.0f, 1.0f);
            } else if (!isHitting) {
                animation.To   = Color.FromScRgb(0.8f, 1.0f, 0.0f, 0.0f);
            } else { /* isRunning && Hitting */
                animation.To   = Color.FromScRgb(0.85f, 0.05f, 1.0f, 0.05f);
                animation.From = Color.FromScRgb(0.75f, 0.1f, 0.9f, 0.1f);
                animation.RepeatBehavior = RepeatBehavior.Forever;
                animation.AutoReverse = true;
            }

            Storyboard stroy = new Storyboard();
            stroy.Children.Add(animation);
            Storyboard.SetTarget(animation, BreathingBorder);
            Storyboard.SetTargetProperty(animation, new PropertyPath("BorderBrush.Color"));
            stroy.Begin();

            /* References:
             * 
             * [Beginner's WPF Animation Tutorial]
             * (http://www.codeproject.com/Articles/23257/Beginner-s-WPF-Animation-Tutorial)
             * [Modify a storyboard to repeat or reverse at the end of its cycle]
             * (https://msdn.microsoft.com/en-us/library/cc294704.aspx?f=255&MSPPError=-2147217396)
             * [Border Color animation in WPF not working in "real-time"]
             * (http://stackoverflow.com/questions/16048401/border-color-animation-in-wpf-not-working-in-real-time)
             * [Borderbrush coloranimation fade in and out]
             * (http://stackoverflow.com/questions/7231093/borderbrush-coloranimation-fade-in-and-out)
             */
        }


        private ActionHelper action = new ActionHelper();

        private System.Threading.Mutex mutex = new System.Threading.Mutex();

        private Timer mainTimer = new Timer()
        {
            Interval = 40,
            Enabled = false,
        };

        /* References:
         * 
         * [Repeating a function every few seconds]
         * (http://stackoverflow.com/questions/11296897/repeating-a-function-every-few-seconds)
         */
    }
}
