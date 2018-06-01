using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Windows.Interop;

namespace Editor
{
    class D3DHwndHost : HwndHost
    {
        public D3DHwndHost()
        {

        }

     /*       vEngine.RenderFrame();
            vEngine.InitD3D(this.Handle, this.Width, this.Height);*/



        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            throw new NotImplementedException();
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            //throw new NotImplementedException();
        }
    }

}