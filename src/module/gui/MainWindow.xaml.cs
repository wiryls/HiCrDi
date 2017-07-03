using System.Timers;
using System.Windows;
using System.Windows.Input;

namespace GUI
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
            // this.IsTesting = true;
            this.main_timer.Elapsed += new ElapsedEventHandler(ProcessFrame);

            try {
                heart = new Utility.Heart();
            } catch (System.DllNotFoundException e) {
                MessageBox.Show
                (
                    e.Message,
                    "加载 DLL 文件时发生错误",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error
                );
                Close();
                return;
            }

            if (!heart.IsReady()) {
                MessageBox.Show
                (
                    "初始化失败，可能是缺少一些必要的配置文件。",
                    "加载文件时发生错误",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error
                );
                Close();
            } else {
                heart.Start();
                heart.Pause();
            }
        }

        private void Window_Close(object sender, RoutedEventArgs e)
        {
            heart.Stop();
            this.main_timer.Close();
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

        private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            this.DebugOutput.Source = null;
        }

        private void Window_StateChanged(object sender, System.EventArgs e)
        {
            /* no maximized window */
            if(this.WindowState == WindowState.Maximized)
                this.WindowState = WindowState.Normal;
        }
    }
}
