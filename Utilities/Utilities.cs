using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using WordCloudSharp;

namespace Utilities
{
    public class Utilities
    {
        public static string GetHtmlByUrl(string url)
        {
            var client = new WebClient {Encoding = Encoding.UTF8};

            var data = client.DownloadData(url);
            var contentType = client.ResponseHeaders["Content-Type"];

            string encoding = null;

            var contentTypeSlices = contentType.Split(new[] {';', ' '}, StringSplitOptions.RemoveEmptyEntries);
            foreach (var contentTypeSlice in contentTypeSlices)
            {
                var slices = contentTypeSlice.Split(new[] {'=', ' '}, StringSplitOptions.RemoveEmptyEntries);
                if (slices[0] == "charset")
                {
                    encoding = slices[1];
                    break;
                }
            }

            if (encoding == null)
                encoding = "UTF-8";

            var webEncoding = Encoding.GetEncoding(encoding);
            client.Dispose();
            return webEncoding.GetString(data);
        }

        public static string GetDecodedHtml(string encodedHtml)
        {
            return WebUtility.HtmlDecode(encodedHtml);
        }

        public static void SaveWordCloud(string csvFilePath, string savingFilePath)
        {
            var wordsAndFreqs = new List<Tuple<string, int>>();

            using (var streamReader = new StreamReader(csvFilePath, Encoding.GetEncoding("gb2312")))
            {
                string reading;
                while ((reading = streamReader.ReadLine()) != null)
                {
                    var slices = reading.Split(',');
                    wordsAndFreqs.Add(new Tuple<string, int>(slices[0], int.Parse(slices[1])));
                }
            }

            wordsAndFreqs.Sort((lhs, rhs) => rhs.Item2.CompareTo(lhs.Item2));

            var wordCloud = new WordCloud(1600, 900, allowVerical: true);
            var image = wordCloud.Draw(wordsAndFreqs.Select(item => item.Item1).ToList(),
                wordsAndFreqs.Select(item => item.Item2).ToList());

            using (var stream = File.OpenWrite(savingFilePath))
            {
                image.Save(stream, ImageFormat.Png);
            }
        }
    }
}