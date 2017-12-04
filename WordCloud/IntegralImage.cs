using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Security.Cryptography;

namespace WordCloudSharp
{
    internal class IntegralImage
    {
        private void InitMask(FastImage image)
        {
            Update(Util.CropImage(image), 1, 1);
        }

        public void Update(FastImage image, int posX, int posY)
        {
            if (posX < 1) posX = 1;
            if (posY < 1) posY = 1;
            var pixelSize = Math.Min(3, image.PixelFormatSize);

            for (var i = posY; i < image.Height; ++i)
            for (var j = posX; j < image.Width; ++j)
            {
                byte pixel = 0;
                for (var p = 0; p < pixelSize; ++p)
                    pixel |= image.Data[i * image.Stride + j * image.PixelFormatSize + p];
                Integral[j, i] = pixel + Integral[j - 1, i] + Integral[j, i - 1] - Integral[j - 1, i - 1];
            }
        }

        public ulong GetArea(int xPos, int yPos, int sizeX, int sizeY)
        {
            ulong area = Integral[xPos, yPos] + Integral[xPos + sizeX, yPos + sizeY];
            area -= Integral[xPos + sizeX, yPos] + Integral[xPos, yPos + sizeY];
            return area;
        }

        #region attributes & constructors

        public int OutputImgWidth { get; set; }

        public int OutputImgHeight { get; set; }

        protected uint[,] Integral { get; set; }

        public IntegralImage(int outputImgWidth, int outputImgHeight)
        {
            Integral = new uint[outputImgWidth, outputImgHeight];
            OutputImgWidth = outputImgWidth;
            OutputImgHeight = outputImgHeight;
        }

        public IntegralImage(FastImage image)
        {
            Integral = new uint[image.Width, image.Height];
            OutputImgWidth = image.Width;
            OutputImgHeight = image.Height;
            InitMask(image);
        }

        #endregion

#if DEBUG

        public Bitmap IntegralImageToBitmap()
        {
            var bmp = new Bitmap(OutputImgWidth, OutputImgHeight, PixelFormat.Format8bppIndexed);
            var bmpdata = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height),
                ImageLockMode.ReadWrite, PixelFormat.Format8bppIndexed);
            var len = bmpdata.Stride * bmp.Height;
            var buffer = new byte[len];
            var bufIndex = 0;
            for (var i = 0; i < OutputImgHeight; ++i)
            for (var j = 0; j < OutputImgWidth; ++j)
            {
                if (i == 0 || j == 0)
                {
                    buffer[bufIndex++] = 255;
                    continue;
                }
                if (Integral[j, i] - Integral[j - 1, i] - Integral[j, i - 1] + Integral[j - 1, i - 1] == 0)
                    buffer[bufIndex++] = 255;
                else
                    buffer[bufIndex++] = 0;
            }
            Marshal.Copy(buffer, 0, bmpdata.Scan0, buffer.Length);
            bmp.UnlockBits(bmpdata);
            return bmp;
        }

        public void SaveIntegralImageToBitmap(string filename = "")
        {
            if (filename == "")
                IntegralImageToBitmap().Save(Path.GetRandomFileName() + ".bmp");
            else
                IntegralImageToBitmap().Save(filename + DateTime.Now.ToString("_hhmmss_fff") + ".bmp");
        }


        public static string GetSha(byte[] array)
        {
            using (var sha1 = new SHA1CryptoServiceProvider())
            {
                return Convert.ToBase64String(sha1.ComputeHash(array));
            }
        }

#endif
    }
}