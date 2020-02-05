namespace FITOM_GUI.UserPanel.VoiceEditPanel
{
    partial class GeneralPanel
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.customFastLabel1 = new FITOM_GUI.CustomControl.CustomFastLabel();
            this.customPictureButton1 = new FITOM_GUI.CustomControl.CustomPictureButton();
            this.SuspendLayout();
            // 
            // customFastLabel1
            // 
            this.customFastLabel1.AutoSize = true;
            this.customFastLabel1.ForeColor = System.Drawing.Color.White;
            this.customFastLabel1.Location = new System.Drawing.Point(56, 51);
            this.customFastLabel1.Name = "customFastLabel1";
            this.customFastLabel1.Size = new System.Drawing.Size(95, 24);
            this.customFastLabel1.TabIndex = 0;
            this.customFastLabel1.Text = "Algorithm";
            // 
            // customPictureButton1
            // 
            this.customPictureButton1.AutoToggle = false;
            this.customPictureButton1.DisabledImage = null;
            this.customPictureButton1.EnabledImage = null;
            this.customPictureButton1.Location = new System.Drawing.Point(157, 51);
            this.customPictureButton1.Name = "customPictureButton1";
            this.customPictureButton1.PressedImage = null;
            this.customPictureButton1.Selected = false;
            this.customPictureButton1.SelectedImage = null;
            this.customPictureButton1.Size = new System.Drawing.Size(308, 23);
            this.customPictureButton1.TabIndex = 1;
            this.customPictureButton1.Text = "customPictureButton1";
            // 
            // GeneralPanel
            // 
            this.Controls.Add(this.customPictureButton1);
            this.Controls.Add(this.customFastLabel1);
            this.Name = "GeneralPanel";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private CustomControl.CustomFastLabel customFastLabel1;
        private CustomControl.CustomPictureButton customPictureButton1;
    }
}
