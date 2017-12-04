using System;
using System.ComponentModel;
using System.Windows;

namespace GraphicsInterface
{
    public partial class MainWindow
    {
        private readonly GuiCore _guiCore = new GuiCore();

        public MainWindow()
        {
            InitializeComponent();
            Initialize();
        }

        private void Initialize()
        {
            InputBox.IsEnabled = false;
            QueryButton.IsEnabled = false;

            var dictionaryLoading = new BackgroundWorker();
            var websiteParsing = new BackgroundWorker();

            StatusText.Text = "Building Dictionaries...";
            InputBox.Text = "Building Dictionaries...";

            dictionaryLoading.DoWork += (sender, args) => { _guiCore.InitializeDictionary(); };

            websiteParsing.DoWork += (sender, args) =>
            {
                _guiCore.ParsingProgressReport += progress =>
                {
                    Progress.Dispatcher.Invoke(() =>
                    {
                        Console.WriteLine(progress);
                        Progress.Value = (int) (progress * Progress.Maximum);
                    });
                };

                _guiCore.ProcessUrls();
            };

            dictionaryLoading.RunWorkerCompleted += (sender, args) =>
            {
                StatusText.Text = "Downloading and parsing websites...";
                InputBox.Text = "Downloading and Parsing websites...";

                websiteParsing.RunWorkerAsync();
            };

            websiteParsing.RunWorkerCompleted += (sender, args) =>
            {
                StatusText.Text = "Ready";
                InputBox.Text = "";
                InputBox.IsEnabled = true;
                QueryButton.IsEnabled = true;
            };

            dictionaryLoading.RunWorkerAsync();
        }

        private void QueryButton_OnClick(object sender, RoutedEventArgs e)
        {
            var queryResults = _guiCore.Query(InputBox.Text);

            ResultDisplay.Text = "";

            foreach (var queryResult in queryResults)
            {
                var documentId = queryResult.Key;
                var occurrence = queryResult.Value;
                ResultDisplay.Text += $"Document ID: {documentId}; Occurence: {occurrence}\n";
            }
        }
    }
}