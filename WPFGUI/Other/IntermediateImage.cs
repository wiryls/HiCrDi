using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace WPFGUI.Other
{
    class IntermediateImage : ICloneable
    {

        public IntermediateImage(BitmapSource img)
        {
            if(img == null)
                throw new System.ArgumentNullException();

            if(img.Format != PixelFormats.Bgra32)
                img = new FormatConvertedBitmap(img, PixelFormats.Bgra32, null, 0);

            imgInfo.Width = img.PixelWidth;
            imgInfo.Height = img.PixelHeight;
            imgInfo.Format = img.Format;
            imgInfo.DpiX = img.DpiX;
            imgInfo.DpiY = img.DpiY;

            /* Stride 的计算方式 */
            imgInfo.Stride = imgInfo.Width * imgInfo.Format.BitsPerPixel / 8;

            /* 保存图片像素信息到 ImgPixels，只包含各个像素的信息 */
            imgPixels = new byte[imgInfo.Height * imgInfo.Stride];
            img.CopyPixels(imgPixels, imgInfo.Stride, 0);
        }

        public IntermediateImage(IntermediateImage rhs)
        {
            if(rhs == null)
                throw new System.ArgumentNullException();

            if(rhs == this)
                return;

            this.imgInfo = rhs.imgInfo;
            this.imgPixels = rhs.imgPixels.Clone() as byte[];
        }

        public BitmapSource ToBitmapSource()
        {
            return BitmapSource.Create(
                imgInfo.Width,
                imgInfo.Height,
                imgInfo.DpiX,
                imgInfo.DpiY,
                imgInfo.Format,
                null,
                imgPixels,
                imgInfo.Stride
                );

            /*
             * References: 
             * [How can I convert byte[] to BitmapImage?]
             * (http://stackoverflow.com/questions/15270844/how-can-i-convert-byte-to-bitmapimage/15272528#15272528) 
             */
        }

        public object Clone()
        {
            return new IntermediateImage(this);
        }

        public byte[] ImagePixels { get { return imgPixels; } }
        public ImageInfo Info { get { return imgInfo; } }

        [StructLayout(LayoutKind.Sequential)]
        public struct ImageInfo
        {
            public double DpiX;
            public double DpiY;
            public int Width;
            public int Height;
            public int Stride;
            public PixelFormat Format;
        }

        private byte[] imgPixels;               /* 数组保存的像素信息 */
        private ImageInfo imgInfo;              /* 与 ImgPixels 对应的，图片信息 */
    }
}
