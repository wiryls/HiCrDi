using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;

namespace WPFGUI
{
    public partial class MainWindow : Window
    {
        private void Window_Close(object sender, RoutedEventArgs e)
        {
            Close();
            e.Handled = true;
            /* References:
             * 
             * [Creating a custom Close Button in WPF]
             * (http://stackoverflow.com/questions/5193763/creating-a-custom-close-button-in-wpf)
             */
        }

        private void Window_Switch(object sender, RoutedEventArgs e)
        {
            SwitchStatus();
            e.Handled = true;
        }

        private void Window_DragMove(object sender, MouseButtonEventArgs e)
        {
            if(e.LeftButton == MouseButtonState.Pressed)
                DragMove();
        }

        private void Window_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if(e.ChangedButton == MouseButton.Left)
                SwitchStatus();

            /* References:
             * 
             * [How do you detect a mouse double left click in WPF?]
             * (http://stackoverflow.com/questions/11868956/how-do-you-detect-a-mouse-double-left-click-in-wpf)
             */
        }

        private void SwitchStatus()
        {
            switch(globalStatus) {
            default:
            case RunningStatus.Settings:
                ChangeStatus(RunningStatus.Running);
                break;
            case RunningStatus.Running:
            case RunningStatus.Capturing:
                ChangeStatus(RunningStatus.Settings);
                break;
            }
        }

        private void ChangeStatus(RunningStatus status)
        {
            if(globalStatus == status)
                return;
            else 
               globalStatus = status;

            mainTimer.Enabled = (
                globalStatus == RunningStatus.Running || 
                globalStatus == RunningStatus.Capturing
                );

            ColorAnimation animation = new ColorAnimation()
            {
                Duration = new Duration(TimeSpan.FromSeconds(0.618)),
                FillBehavior = FillBehavior.HoldEnd
            };

            switch(globalStatus) {
            default:
            case RunningStatus.Settings:
                animation.To = Color.FromScRgb(0.8f, 1.0f, 1.0f, 1.0f);
                break;
            case RunningStatus.Running:
                animation.To = Color.FromScRgb(0.8f, 1.0f, 0.0f, 0.0f);
                break;
            case RunningStatus.Capturing:
                animation.To = Color.FromScRgb(0.85f, 0.05f, 1.0f, 0.05f);
                animation.From = Color.FromScRgb(0.75f, 0.1f, 0.9f, 0.1f);
                animation.RepeatBehavior = RepeatBehavior.Forever;
                animation.AutoReverse = true;
                break;
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

        private enum RunningStatus
        {
            Settings,
            Capturing,
            Running,
        }

        private RunningStatus globalStatus = RunningStatus.Settings;
    }
}
