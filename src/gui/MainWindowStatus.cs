using System.ComponentModel;
using System.Windows;

namespace GUI
{
    public partial class MainWindow : Window, INotifyPropertyChanged
    {

        public bool IsRunning
        {
            get { return this.isRunning; }
            set
            {
                if(this.isRunning != value) {
                    this.isRunning = value;
                    this.mainTimer.Enabled = this.isRunning;
                    if(!this.isRunning) {
                        this.isHitting = false;
                        heart.Pause();
                        this.ResizeMode = ResizeMode.CanResizeWithGrip;
                    } else {
                        this.ResizeMode = ResizeMode.NoResize;
                        heart.Resume();
                    }
                    this.UpdateBorderEffect();
                    OnPropertyChanged("IsRunning");
                }
            }
        }

        public bool IsTesting
        {
            get { return this.isTesting; }
            set
            {
                if(this.isTesting != value) {
                    this.isTesting = value;
                    if(this.isTesting) {
                        var corner = this.BreathingBorder.CornerRadius;
                        corner.BottomRight = corner.TopRight;
                        this.BreathingBorder.CornerRadius = corner;

                        this.View.RowDefinitions[1].Height = new GridLength(1, GridUnitType.Star);
                        this.MaxHeight = this.MaxHeight * 2.0;
                        this.Height = this.Height * 2.0;
                    } else {
                        var corner = this.BreathingBorder.CornerRadius;
                        corner.BottomRight = 0;
                        this.BreathingBorder.CornerRadius = corner;

                        this.View.RowDefinitions[1].Height = new GridLength(0, GridUnitType.Star);
                        this.Height = this.Height * 0.5;
                        this.MaxHeight = this.MaxHeight * 0.5;
                    }
                    OnPropertyChanged("IsTesting");
                }
            }
        }

        public bool IsAutoScreenshot
        {
            get { return this.isAutoScreenshot; }
            set
            {
                this.isAutoScreenshot = value;
                OnPropertyChanged("IsAutoScreenshot");
            }
        }

        public bool IsKeepingIdle
        {
            get { return this.isKeepingIdle; }
            set
            {
                this.isKeepingIdle = value;
                OnPropertyChanged("IsKeepingIdle");
            }
        }

        public bool IsAutoRestart
        {
            get { return this.isAutoRestart; }
            set
            {
                this.isAutoRestart = value;
                OnPropertyChanged("IsAutoRestart");
            }
        }



        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if(handler != null)
                handler(this, new PropertyChangedEventArgs(propertyName));

            /* References:
             *
             * [WPF checkbox binding]
             * (http://stackoverflow.com/questions/870163/wpf-checkbox-binding)
             * [WPF - OnPropertyChanged for a property within a collection]
             * (http://stackoverflow.com/questions/956165/wpf-onpropertychanged-for-a-property-within-a-collection)
             * [OnPropertyChanged and XAML]
             * (http://stackoverflow.com/questions/12895184/onpropertychanged-and-xaml)
             */
        }

        private bool isAutoRestart    = true;
        private bool isAutoScreenshot = false;
        private bool isKeepingIdle    = false;
        private bool isTesting = false;
        private bool isRunning = false;
        private bool isHitting = false;
    }
}
