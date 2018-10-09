using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
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

namespace Editor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private MainViewModel vm;
        public MainWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(this.WindowLoaded);
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            // Create our OpenGL Hwnd 'control'...
            //System.Windows.Interop.HwndHost host = new D3DHwndHost();

            // ... and attach it to the placeholder control:
            //D3DHwndHost.Child = host;
            //vEngine.InitD3D((new System.Windows.Interop.WindowInteropHelper(this)).Handle, 200,300);
            //vEngine.RenderFrame();

            //             vm = new MainViewModel
            //             {
            //                 Content = new D3DHwndHost()
            //             };

            insertHwndHostHere.Child = new D3DHwndHost();

            InitializeComponent();

            //DataContext = vm;
        }


    }
}
