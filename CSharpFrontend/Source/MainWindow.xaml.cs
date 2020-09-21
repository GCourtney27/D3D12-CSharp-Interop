using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;
using System.Diagnostics;
using NativeCodeWrapper;

namespace DX11OnCSharp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport("user32.dll", SetLastError = true)]
        static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        public MainWindow()
        {
            InitializeComponent();
            
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            String Msg = "Hello World";
            Renderer renderer = new Renderer();
            IntPtr hWnd = (IntPtr)FindWindow("MainWindow", null);

            renderer.Init(hWnd);
            while(true)
            {
                renderer.RenderFrame();
            }
            //renderer.Init()
            //renderer.SendMessage(Msg);
        }
    }
}
