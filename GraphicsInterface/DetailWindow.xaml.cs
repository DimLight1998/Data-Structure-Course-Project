using System;
using System.Windows.Documents;

namespace GraphicsInterface
{
    public partial class DetailWindow
    {
        public DetailWindow(int documentId, int occurrence, MainWindow mainWindow)
        {
            InitializeComponent();

            var inlines = ContentBox.Inlines;

            inlines.Clear();
            var stringList = mainWindow.InputBox.Text.Split(new[] {' '}, StringSplitOptions.RemoveEmptyEntries);

            // Add Information.
            inlines.Add(new Run($"ID of this document : {documentId}"));
            inlines.Add(new LineBreak());
            inlines.Add(new Run($"Occurrence in this document : {occurrence}"));
            inlines.Add(new LineBreak());
            inlines.Add(new LineBreak());

            inlines.Add(MainWindow.GetFormattedString(mainWindow.Core.GetPostTitle(documentId), stringList));

            inlines.Add(new LineBreak());
            inlines.Add(new LineBreak());

            inlines.Add(MainWindow.GetFormattedString(mainWindow.Core.GetPostContent(documentId), stringList));
        }
    }
}