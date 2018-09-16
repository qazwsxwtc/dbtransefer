using System.Threading;
namespace WindowsFormsApplication1
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.logon = new System.Windows.Forms.Button();
            this.ConnectDb = new System.Windows.Forms.Button();
            this.ReadDb = new System.Windows.Forms.Button();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // logon
            // 
            this.logon.Location = new System.Drawing.Point(113, 27);
            this.logon.Name = "logon";
            this.logon.Size = new System.Drawing.Size(75, 23);
            this.logon.TabIndex = 0;
            this.logon.Text = "logon";
            this.logon.UseVisualStyleBackColor = true;
            this.logon.Click += new System.EventHandler(this.logon_Click);
            // 
            // ConnectDb
            // 
            this.ConnectDb.Location = new System.Drawing.Point(211, 27);
            this.ConnectDb.Name = "ConnectDb";
            this.ConnectDb.Size = new System.Drawing.Size(75, 23);
            this.ConnectDb.TabIndex = 1;
            this.ConnectDb.Text = "ConnectDb";
            this.ConnectDb.UseVisualStyleBackColor = true;
            this.ConnectDb.Click += new System.EventHandler(this.ConnectDb_Click);
            // 
            // ReadDb
            // 
            this.ReadDb.Location = new System.Drawing.Point(392, 12);
            this.ReadDb.Name = "ReadDb";
            this.ReadDb.Size = new System.Drawing.Size(75, 23);
            this.ReadDb.TabIndex = 2;
            this.ReadDb.Text = "ReadDb";
            this.ReadDb.UseVisualStyleBackColor = true;
            this.ReadDb.Click += new System.EventHandler(this.ReadDb_Click);
            // 
            // richTextBox1
            // 
            this.richTextBox1.Location = new System.Drawing.Point(12, 69);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(553, 237);
            this.richTextBox1.TabIndex = 4;
            this.richTextBox1.Text = "";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(32, 12);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 5;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(577, 318);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.richTextBox1);
            this.Controls.Add(this.ReadDb);
            this.Controls.Add(this.ConnectDb);
            this.Controls.Add(this.logon);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);

        }

        


        #endregion

        private System.Windows.Forms.Button logon;
        private System.Windows.Forms.Button ConnectDb;
        private System.Windows.Forms.Button ReadDb;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.Button button1;
    }
}

