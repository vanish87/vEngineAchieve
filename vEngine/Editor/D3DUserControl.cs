using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Editor
{
    class D3DUserControl : UserControl
    {
        public D3DUserControl()
        {
            this.InitializeComponent();
        }
        protected override void OnPaint(PaintEventArgs e)
        {
            vEngine.RenderFrame();
        }

        protected override void OnResize(EventArgs e)
        {
            vEngine.InitD3D(this.Handle, this.Width, this.Height);
        }


        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // D3DUserControl
            // 
            this.Name = "D3DUserControl";
            this.Load += new System.EventHandler(this.D3DUserControl_Load);
            this.ResumeLayout(false);

        }
        private void D3DUserControl_Load(object sender, EventArgs e)
        {
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