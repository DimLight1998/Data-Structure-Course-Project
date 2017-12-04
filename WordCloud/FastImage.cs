using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace WordCloudSharp
{
    /// <summary>
    ///     Image class for fast manipulation of bitmap data.
    /// </summary>
#if DEBUG
    public
#else
    internal
#endif
        class FastImage : IDisposable
    {
        public FastImage(int width, int height, PixelFormat format)
        {
            PixelFormatSize = Image.GetPixelFormatSize(format) / 8;
            Stride = width * PixelFormatSize;

            Data = new byte[Stride * height];
            Handle = GCHandle.Alloc(Data, GCHandleType.Pinned);
            var pData = Marshal.UnsafeAddrOfPinnedArrayElement(Data, 0);
            Bitmap = new Bitmap(width, height, Stride, format, pData);
        }

        public FastImage(Image img) : this(img.Width, img.Height, img.PixelFormat)
        {
            var bmp = new Bitmap(img);
            var bmpdatain = bmp.LockBits(new Rectangle(0, 0, Bitmap.Width, Bitmap.Height), ImageLockMode.ReadOnly,
                Bitmap.PixelFormat);
            Marshal.Copy(bmpdatain.Scan0, Data, 0, Data.Length);
            bmp.UnlockBits(bmpdatain);
        }

        public int Width => Bitmap.Width;

        public int Height => Bitmap.Height;

        public int PixelFormatSize { get; set; }

        public GCHandle Handle { get; set; }

        public int Stride { get; set; }

        public byte[] Data { get; set; }

        public Bitmap Bitmap { get; set; }

        public void Dispose()
        {
            Handle.Free();
            Bitmap.Dispose();
        }
    }
}