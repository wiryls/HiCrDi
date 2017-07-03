using System;
using System.Timers;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using GUI.Utility;

namespace GUI
{
    public partial class MainWindow : Window
    {
        private void ProcessFrame(object source, ElapsedEventArgs e)
        {
            mutex01.WaitOne();

            var bitmap = this.Dispatcher.Invoke(()=> {
                return ImageHelper.CaptureElement(this.MainFrame);
            });
            if(bitmap == null) {
                mutex01.ReleaseMutex();
                return;
            }

            mutex02.WaitOne();

            BitmapSource bitmapSource = ImageHelper.Bitmap2BitmapSource(bitmap);

            mutex01.ReleaseMutex();

            var ok = heart.TakeImage(bitmapSource);

            if(isTesting) {
                var dbg_img = heart.GetDebugImage();

                if(isTesting) {
                    dbg_img.Freeze();
                    this.Dispatcher.BeginInvoke(new Action<BitmapSource>((img) =>
                        this.DebugOutput.Source = img
                    ), dbg_img);
                }
            }

            if(isHitting != ok) {
                isHitting = ok;
                this.Dispatcher.Invoke(() => { UpdateBorderEffect(); });
            }

            mutex02.ReleaseMutex();

            if (heart.IsGameOver()) {
                if (this.isDead == false && this.IsAutoScreenshot == true) {
                    heart.Pause();
                    var folder_path = "screenshot" + "\\" + DateTime.Now.ToString("HHmmss") + ".png";
                    ImageHelper.SaveImgFile(bitmapSource, folder_path);
                    heart.Resume();
                }
                if (this.IsAutoRestart == true) {
                    this.action.Jump();
                    this.action.Idle();
                }

                if (this.isDead == false)
                    this.isDead = true;
            } else {
                if (this.IsKeepingIdle == false) {
                    switch(heart.Choice()) {
                    case Heart.Action.Idle: { this.action.Idle(); break; }
                    case Heart.Action.Jump: { this.action.Jump(); break; }
                    case Heart.Action.Down: { this.action.Down(); break; }
                    default:                { this.action.Idle(); break; }
                    }
                }

                if (this.isDead == true)
                    this.isDead = false;
            }

            if (this.isDead == false && this.isHitting == true) {
                if (main_timer.Interval != timer_running_interval)
                    main_timer.Interval = timer_running_interval;
            } else {
                if (main_timer.Interval != timer_relaxing_interval)
                    main_timer.Interval = timer_relaxing_interval;
            }

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
            var time = TimeSpan.FromSeconds(0.618);
            var color_idle      = Color.FromScRgb(0.8f, 1.0f, 1.0f, 1.0f);
            var color_searching = Color.FromScRgb(0.8f, 0.8f, 0.1f, 0.1f);
            var color_hitting_l = Color.FromScRgb(0.8f, 0.05f, 0.6f, 0.05f);
            var color_hitting_h = Color.FromScRgb(0.8f, 0.1f, 0.9f, 0.1f);

            Storyboard story = new Storyboard();

            if (!isRunning) {
                story.Children.Add(new ColorAnimation
                {
                    Duration = new Duration(time),
                    FillBehavior = FillBehavior.HoldEnd,
                    To = color_idle
                });
            } else if (!isHitting) {
                story.Children.Add(new ColorAnimation
                {
                    Duration = new Duration(time),
                    FillBehavior = FillBehavior.HoldEnd,
                    To = color_searching
                });
            } else { /* isRunning && Hitting */
                story.Children.Add(new ColorAnimation
                {
                    Duration = new Duration(time),
                    FillBehavior = FillBehavior.HoldEnd,
                    To = color_hitting_l,
                });

                story.Children.Add(new ColorAnimation
                {
                    BeginTime = time,
                    Duration = new Duration(TimeSpan.FromSeconds(0.618 * 2)),
                    FillBehavior = FillBehavior.HoldEnd,
                    From = color_hitting_l,
                    To   = color_hitting_h,
                    RepeatBehavior = RepeatBehavior.Forever,
                    AutoReverse = true
                });
            }

            foreach(var animation in story.Children) {
                Storyboard.SetTarget(animation, BreathingBorder);
                Storyboard.SetTargetProperty(animation, new PropertyPath("BorderBrush.Color"));
            }
            story.Begin();

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

        private Utility.Heart heart;

        private ActionHelper action = new ActionHelper();

        private System.Threading.Mutex mutex01 = new System.Threading.Mutex();
        private System.Threading.Mutex mutex02 = new System.Threading.Mutex();

        private const double timer_running_interval  = 25;
        private const double timer_relaxing_interval = 500;

        private Timer main_timer = new Timer()
        {
            Interval = timer_relaxing_interval,
            Enabled = false,
        };

        private bool isDead = false;

        /* References:
         *
         * [Repeating a function every few seconds]
         * (http://stackoverflow.com/questions/11296897/repeating-a-function-every-few-seconds)
         */
    }
}
