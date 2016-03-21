using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace WPFGUI
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
                    if(!this.isRunning)
                        this.isHitting = false;
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
                        this.View.RowDefinitions[1].Height = new GridLength(1, GridUnitType.Star);
                        this.MaxHeight = this.MaxHeight * 2.0;
                        this.Height = this.Height * 2.0;
                    } else {
                        this.View.RowDefinitions[1].Height = new GridLength(0, GridUnitType.Star);
                        this.Height = this.Height * 0.5;
                        this.MaxHeight = this.MaxHeight * 0.5;
                    }
                    OnPropertyChanged("IsTesting");
                }
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

        private bool isTesting = false;
        private bool isRunning = false;
        private bool isHitting = false;
    }
}
