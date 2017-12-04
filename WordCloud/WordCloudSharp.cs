using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Text;
using System.Threading;

namespace WordCloudSharp
{
    /// <summary>
    ///     Class to draw word clouds.
    /// </summary>
    public class WordCloud
    {
        /// <summary>
        ///     Initializes a new instance of the <see cref="WordCloud" /> class.
        /// </summary>
        /// <param name="width">The width of word cloud.</param>
        /// <param name="height">The height of word cloud.</param>
        /// <param name="useRank">if set to <c>true</c> will ignore frequencies for best fit.</param>
        /// <param name="fontColor">Color of the font.</param>
        /// <param name="maxFontSize">Maximum size of the font.</param>
        /// <param name="fontStep">The font step to use.</param>
        /// <param name="mask">mask image</param>
        /// <param name="allowVerical">allow vertical text</param>
        /// <param name="fontname">name of the font</param>
        public WordCloud(int width, int height, bool useRank = false, Color? fontColor = null, float maxFontSize = -1,
            int fontStep = 1, Image mask = null, bool allowVerical = false, string fontname = null)
        {
            if (mask == null)
            {
                Map = new OccupancyMap(width, height);
                WorkImage = new FastImage(width, height, PixelFormat.Format32bppArgb);
            }
            else
            {
                mask = Util.ResizeImage(mask, width, height);
                if (!Util.CheckMaskValid(mask))
                    throw new Exception("Mask is not a valid black-white image");
                Map = new OccupancyMap(mask);
                WorkImage = new FastImage(mask);
            }

            MaxFontSize = maxFontSize < 0 ? height : maxFontSize;
            FontStep = fontStep;
            _mFontColor = fontColor;
            UseRank = useRank;
            Random = new Random(Environment.TickCount);
            AllowVertical = allowVerical;
            Fontname = fontname;
#if DEBUG
            _drawWaitHandle = new AutoResetEvent(false);
            StepDrawMode = false;
#endif
        }

        public event Action<double> OnProgress;

        /// <summary>
        ///     Gets a random color.
        /// </summary>
        /// <returns>Color</returns>
        private Color GetRandomColor()
        {
            return Color.FromArgb(Random.Next(0, 255), Random.Next(0, 255), Random.Next(0, 255));
        }

        /// <summary>
        ///     Draws the specified word cloud given list of words and frequecies
        /// </summary>
        /// <param name="words">List of words ordered by occurance.</param>
        /// <param name="freqs">List of frequecies.</param>
        /// <param name="bgcolor">Backgroud color of the output image</param>
        /// <param name="img">Backgroud image of the output image</param>
        /// <returns>Image of word cloud.</returns>
        /// <exception cref="System.ArgumentException">
        ///     Arguments null.
        ///     or
        ///     Must have the same number of words as frequencies.
        /// </exception>
        private Image Draw(IList<string> words, IList<int> freqs, Color bgcolor, Image img)
        {
#if DEBUG
            ShowIntegralImgStepDraw(Map.IntegralImageToBitmap());
            _drawWaitHandle.Reset();
#endif
            var fontSize = MaxFontSize;
            if (words == null || freqs == null)
                throw new ArgumentException("Arguments null.");
            if (words.Count != freqs.Count)
                throw new ArgumentException("Must have the same number of words as frequencies.");

            Bitmap result;
            if (img == null)
            {
                result = new Bitmap(WorkImage.Width, WorkImage.Height);
            }
            else
            {
                if (img.Size != WorkImage.Bitmap.Size)
                    throw new Exception("The backgroud img should be with the same size with the mask");
                result = new Bitmap(img);
            }

            using (var gworking = Graphics.FromImage(WorkImage.Bitmap))
            using (var gresult = Graphics.FromImage(result))
            {
                if (img == null)
                    gresult.Clear(bgcolor);
                gresult.TextRenderingHint = TextRenderingHint.AntiAlias;
                gworking.TextRenderingHint = TextRenderingHint.AntiAlias;
                var lastProgress = 0.0d;
                for (var i = 0; i < words.Count; ++i)
                {
                    var progress = (double) i / words.Count;
                    if (progress - lastProgress > 0.01)
                    {
                        ShowProgress(progress);
                        lastProgress = progress;
                    }
                    if (!UseRank)
                        fontSize = (float) Math.Min(fontSize, 100 * Math.Log10(freqs[i] + 100));

                    var format = new StringFormat();
                    if (AllowVertical)
                        if (Random.Next(0, 2) == 1)
                            format.FormatFlags = StringFormatFlags.DirectionVertical;

                    Point p;
                    bool foundPosition;
                    Font font;
                    Debug.WriteLine("Word: " + words[i]);
                    do
                    {
                        font = new Font(Fontname, fontSize, GraphicsUnit.Pixel);
                        var size = gworking.MeasureString(words[i], font, new PointF(0, 0), format);
                        Debug.WriteLine("Search with font size: " + fontSize);
                        foundPosition = Map.GetRandomUnoccupiedPosition((int) size.Width, (int) size.Height, out p);
                        if (!foundPosition) fontSize -= FontStep;
                    } while (fontSize > 0 && !foundPosition);
                    Debug.WriteLine("Found pos: " + p);
                    if (fontSize <= 0) break;
                    gworking.DrawString(words[i], font, new SolidBrush(FontColor), p.X, p.Y, format);
                    gresult.DrawString(words[i], font, new SolidBrush(FontColor), p.X, p.Y, format);
                    Map.Update(WorkImage, p.X, p.Y);
#if DEBUG
                    if (StepDrawMode)
                    {
                        ShowResultStepDraw(new Bitmap(WorkImage.Bitmap));
                        ShowIntegralImgStepDraw(Map.IntegralImageToBitmap());
                        _drawWaitHandle.WaitOne();
                    }
#endif
                }
            }
            WorkImage.Dispose();
            return result;
        }

        /// <summary>
        ///     Draws the specified word cloud with background color spicified given list of words and frequecies
        /// </summary>
        /// <param name="words">List of words ordered by occurance.</param>
        /// <param name="freqs">List of frequecies.</param>
        /// <param name="bgcolor">Specified background color</param>
        /// <returns>Image of word cloud.</returns>
        public Image Draw(IList<string> words, IList<int> freqs, Color bgcolor)
        {
            return Draw(words, freqs, bgcolor, null);
        }

        /// <summary>
        ///     Draws the specified word cloud with background spicified given list of words and frequecies
        /// </summary>
        /// <param name="words">List of words ordered by occurance.</param>
        /// <param name="freqs">List of frequecies.</param>
        /// <param name="img">Specified background image</param>
        /// <returns>Image of word cloud.</returns>
        public Image Draw(IList<string> words, IList<int> freqs, Image img)
        {
            return Draw(words, freqs, Color.White, img);
        }

        /// <summary>
        ///     Draws the specified word cloud with given list of words and frequecies
        /// </summary>
        /// <param name="words">List of words ordered by occurance.</param>
        /// <param name="freqs">List of frequecies.</param>
        /// <returns>Image of word cloud.</returns>
        public Image Draw(IList<string> words, IList<int> freqs)
        {
            return Draw(words, freqs, Color.White, null);
        }

        private void ShowProgress(double progress)
        {
            OnProgress?.Invoke(progress);
        }
#if DEBUG
        public event Action<Image> OnStepDrawResultImg;

        public event Action<Image> OnStepDrawIntegralImg;

        private readonly AutoResetEvent _drawWaitHandle;

        private bool StepDrawMode { get; set; }
#endif

        #region attributes

        /// <summary>
        ///     Gets font colour or random if font wasn't set
        /// </summary>
        private Color FontColor
        {
            get => _mFontColor ?? GetRandomColor();
        }


        private Color? _mFontColor;


        /// <summary>
        ///     Used to select random colors.
        /// </summary>
        private Random Random { get; }


        /// <summary>
        ///     Working image.
        /// </summary>
        private FastImage WorkImage { get; }


        /// <summary>
        ///     Keeps track of word positions using integral image.
        /// </summary>
        private OccupancyMap Map { get; }


        /// <summary>
        ///     Gets or sets the maximum size of the font.
        /// </summary>
        private float MaxFontSize { get; }


        /// <summary>
        ///     User input order instead of frequency
        /// </summary>
        private bool UseRank { get; }

        /// <summary>
        ///     Amount to decrement font size each time a word won't fit.
        /// </summary>
        private int FontStep { get; }

        /// <summary>
        ///     If allow vertical drawing
        /// </summary>
        private bool AllowVertical { get; }

        private string Fontname
        {
            get => _fontname ?? "Microsoft Sans Serif";
            set
            {
                _fontname = value;
                if (value == null) return;
                using (var f = new Font(value, 12, FontStyle.Regular))
                {
                    _fontname = f.FontFamily.Name;
                }
            }
        }

        private string _fontname;

        #endregion

#if DEBUG
        private void ShowResultStepDraw(Image bmp)
        {
            OnStepDrawResultImg?.Invoke(bmp);
        }

        private void ShowIntegralImgStepDraw(Image bmp)
        {
            OnStepDrawIntegralImg?.Invoke(bmp);
        }

        public void ContinueDrawing()
        {
            _drawWaitHandle.Set();
        }
#endif
    }
}