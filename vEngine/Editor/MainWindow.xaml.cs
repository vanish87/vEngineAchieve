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
        public MainWindow()
        {
            InitializeComponent();
            this.D3DRenderWindow.Loaded += new RoutedEventHandler(this.WindowLoaded);
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            vEngine.RenderFrame();
            vEngine.InitD3D((new System.Windows.Interop.WindowInteropHelper(this)).Handle, 200,300);
        }


    }

    public class vEngine
    {
        [DllImport("EditorDll.dll")]
        public static extern void InitD3D(IntPtr window, int Width, int Height);

        [DllImport("EditorDll.dll")]
        public static extern void RenderFrame();

        [DllImport("EditorDll.dll")]
        public static extern void CleanD3D();
    }
}
