namespace JDC.CustomControl
{
    partial class CustomTextBoxBase
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region コンポーネント デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // CustomControl_TextBoxBase
            // 
            this.Font = new System.Drawing.Font("Meiryo UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.EnabledChanged += new System.EventHandler(this.CustomControl_TextBoxBase_EnabledChanged);
            this.SizeChanged += new System.EventHandler(this.CustomControl_TextBoxBase_SizeChanged);
            this.Click += new System.EventHandler(this.OnMouseClick);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.CustomControl_TextBoxBase_MouseDown);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.CustomControl_TextBoxBase_MouseUp);
            this.ResumeLayout(false);

        }

        #endregion
    }
}
