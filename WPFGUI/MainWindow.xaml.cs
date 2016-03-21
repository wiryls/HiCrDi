using System.Timers;
using System.Windows;
using System.Windows.Input;

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
            this.View.DataContext = this;
            // SwitchTest();
            this.mainTimer.Elapsed += new ElapsedEventHandler(ProcessFrame);
        }

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

        private void Window_DragMove(object sender, MouseButtonEventArgs e)
        {
            if(e.LeftButton == MouseButtonState.Pressed) {
                DragMove();
                e.Handled = true;
            }
        }

        private void Window_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if(e.ChangedButton == MouseButton.Left) {
                this.IsRunning = !this.IsRunning;
                e.Handled = true;
            }

            /* References:
             * 
             * [How do you detect a mouse double left click in WPF?]
             * (http://stackoverflow.com/questions/11868956/how-do-you-detect-a-mouse-double-left-click-in-wpf)
             */
        }
    }
}
