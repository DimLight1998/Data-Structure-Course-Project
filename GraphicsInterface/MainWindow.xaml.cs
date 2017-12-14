using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace GraphicsInterface
{
    public partial class MainWindow
    {
        public readonly GuiCore Core = new GuiCore();

        public MainWindow()
        {
            InitializeComponent();
            Initialize();
        }

        private void Initialize()
        {
            InputBox.IsEnabled = false;
            QueryButton.IsEnabled = false;
            ResultDisplay.IsEnabled = false;

            var dictionaryLoading = new BackgroundWorker();
            var websiteParsing = new BackgroundWorker();

            StatusText.Text = "Building Dictionaries...";
            InputBox.Text = "Building Dictionaries...";

            dictionaryLoading.DoWork += (sender, args) => { Core.InitializeDictionary(); };

            websiteParsing.DoWork += (sender, args) =>
            {
                Core.ParsingProgressReport += progress =>
                {
                    Progress.Dispatcher.Invoke(() =>
                    {
                        Console.WriteLine(progress);
                        Progress.Value = (int) (progress * Progress.Maximum);
                    });
                };

                Core.ProcessUrls();
            };

            dictionaryLoading.RunWorkerCompleted += (sender, args) =>
            {
                StatusText.Text = "Downloading and parsing websites...";
                InputBox.Text = "Downloading and parsing websites...";

                websiteParsing.RunWorkerAsync();
            };

            websiteParsing.RunWorkerCompleted += (sender, args) =>
            {
                StatusText.Text = "Ready";
                InputBox.Text = "";
                InputBox.IsEnabled = true;
                QueryButton.IsEnabled = true;
                ResultDisplay.IsEnabled = true;
                Progress.Value = 0;
                Core.DestroyDictionary();
            };

            dictionaryLoading.RunWorkerAsync();
        }

        private void QueryButton_OnClick(object sender, RoutedEventArgs e)
        {
            QueryButton.IsEnabled = false;
            ResultDisplay.Text = "";

            var queryResults = Core.Query(InputBox.Text);

            foreach (var queryResult in queryResults)
            {
                var documentId = queryResult.Key;
                var occurrence = queryResult.Value;

                var button = new Button {Content = "Details..."};
                button.Click += (o, args) =>
                {
                    var detailsWindow = new DetailWindow(documentId, occurrence, this);
                    detailsWindow.Show();
                };

                ResultDisplay.Inlines.Add(button);
                ResultDisplay.Inlines.Add(new LineBreak());
                var stringList = InputBox.Text.Split(new[] {' '}, StringSplitOptions.RemoveEmptyEntries);
                ResultDisplay.Inlines.Add(GetFormattedString(Core.GetPostTitle(documentId), stringList));
                ResultDisplay.Inlines.Add(new LineBreak());
                ResultDisplay.Inlines.Add(new LineBreak());
            }

            QueryButton.IsEnabled = true;
        }

        public static Inline GetFormattedString(string targetString, string[] stringList)
        {
            var hightlightList = new Dictionary<int, int>();

            if (targetString == "")
                return new Span();

            foreach (var word in stringList)
            {
                var indexes = AllIndexesOf(targetString, word);
                foreach (var index in indexes)
                    hightlightList.Add(index, word.Length);
            }

            var isHighlighted = new bool[targetString.Length];
            for (var i = 0; i < isHighlighted.Length; i++)
                isHighlighted[i] = false;

            foreach (var key in hightlightList.Keys)
                for (var i = key; i < key + hightlightList[key]; i++)
                    isHighlighted[i] = true;

            var ret = new Span();

            var lastIndex = 0;
            var renderingIndex = 0;
            var isHighlighting = isHighlighted[0];

            while (renderingIndex < targetString.Length)
                if (isHighlighting == isHighlighted[renderingIndex])
                {
                    renderingIndex++;
                }
                else
                {
                    if (isHighlighting)
                        ret.Inlines.Add(new Bold(
                            new Run(targetString.Substring(lastIndex, renderingIndex - lastIndex))
                            {
                                Background = Brushes.DarkOrange
                            }));
                    else
                        ret.Inlines.Add(new Run(targetString.Substring(lastIndex, renderingIndex - lastIndex)));

                    isHighlighting = !isHighlighting;
                    lastIndex = renderingIndex;
                }

            if (isHighlighting)
                ret.Inlines.Add(new Bold(
                    new Run(targetString.Substring(lastIndex, renderingIndex - lastIndex))
                    {
                        Background = Brushes.DarkOrange
                    }));
            else
                ret.Inlines.Add(new Run(targetString.Substring(lastIndex, renderingIndex - lastIndex)));

            return ret;
        }

        private static IEnumerable<int> AllIndexesOf(string source, string target)
        {
            var indexes = new List<int>();
            for (var index = 0;; index += target.Length)
            {
                index = source.IndexOf(target, index, StringComparison.Ordinal);
                if (index == -1)
                    return indexes;
                indexes.Add(index);
            }
        }
    }
}