using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace WPFGUI.Other
{
    public static class ImageHelper
    {
        public static System.Drawing.Bitmap CaptureElement(FrameworkElement target)
        {
            if(target == null)
                return null;

            var rect = VisualTreeHelper.GetDescendantBounds(target);
            Point screenPosition = target.PointToScreen(new Point(rect.X, rect.Y));

            var bmp = new System.Drawing.Bitmap(
                (int)rect.Width, (int)rect.Height, 
                System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            var g = System.Drawing.Graphics.FromImage(bmp);
            g.CopyFromScreen(
                (int)screenPosition.X, (int)screenPosition.Y, 0, 0, 
                bmp.Size, System.Drawing.CopyPixelOperation.SourceCopy);
            
            return bmp;

            /* References:
             * 
             * [C#: how to take a screenshot of a portion of screen]
             * (http://stackoverflow.com/questions/3306600/c-how-to-take-a-screenshot-of-a-portion-of-screen)
             * [WPF Screen Capture (with Rectangle)]
             * (https://social.msdn.microsoft.com/Forums/vstudio/en-US/7a3d2cee-2e72-420d-b596-d51f7002a07e/wpf-screen-capture-with-rectangle?forum=wpf)
             */
        }

        public static BitmapSource Bitmap2BitmapSource(System.Drawing.Bitmap bitmap)
        {
            var bitmapData = bitmap.LockBits(
                new System.Drawing.Rectangle(0, 0, bitmap.Width, bitmap.Height),
                System.Drawing.Imaging.ImageLockMode.ReadOnly, 
                bitmap.PixelFormat);

            var bitmapSource = BitmapSource.Create(
                bitmapData.Width, bitmapData.Height, 96, 96, PixelFormats.Bgra32, null,
                bitmapData.Scan0, bitmapData.Stride * bitmapData.Height, bitmapData.Stride);

            bitmap.UnlockBits(bitmapData);
            return bitmapSource;

            /* 
             * References:
             *
             * [fast converting Bitmap to BitmapSource wpf]
             * (http://stackoverflow.com/questions/30727343/fast-converting-bitmap-to-bitmapsource-wpf)
             */
        }

        public static void GenerateImage(BitmapSource bitmap, string lowerExtension, Stream destStream)
        {
            BitmapEncoder encoder = null;
            switch(lowerExtension) {
            case ".jpg": { encoder = new JpegBitmapEncoder(); break; }
            case ".png": { encoder = new PngBitmapEncoder (); break; }
            case ".bmp": { encoder = new BmpBitmapEncoder (); break; }
            case ".gif": { encoder = new GifBitmapEncoder (); break; }
            case ".tif": { encoder = new TiffBitmapEncoder(); break; }
            default:     { encoder = new BmpBitmapEncoder (); break; }
            }
            encoder.Frames.Add(BitmapFrame.Create(bitmap));
            encoder.Save(destStream);

            /* 
             * References:
             *
             * [在WPF程序中将控件所呈现的内容保存成图像]
             * (http://www.cnblogs.com/TianFang/archive/2012/10/07/2714140.html) 
             * [Rendering a WPF Container to Bitmap]
             * (http://weblog.west-wind.com/posts/2007/Sep/10/Rendering-a-WPF-Container-to-Bitmap)
             */
        }

        public static void SaveImgFile(BitmapSource img)
        {
            /*
             * References:
             *
             * [How do I show a Save As dialog in WPF?]
             * (http://stackoverflow.com/questions/5622854/how-do-i-show-a-save-as-dialog-in-wpf)
             */

            var dlg = new Microsoft.Win32.SaveFileDialog()
            {
                DefaultExt = "*.bmp|*.png|*.jpg|*.gif|*tif",
                Filter = "Bitmap (*.bmp)|*.bmp"
                       + "|PNG image (*.png)|*.png"
                       + "|JPEG image (*.jpg)|*.jpg"
                       + "|GIF image (*.gif)|*.gif"
                       + "|TIF image (*.tif)|*.tif"
            };

            var result = dlg.ShowDialog();
            if(result == true) {
                var filename = dlg.FileName;
                var Extension = System.IO.Path.GetExtension(filename).ToLower();
                var fs = File.Open(filename, FileMode.Create);

                ImageHelper.GenerateImage(img, Extension, fs);
                fs.Close();
            }
        }
    }
}
