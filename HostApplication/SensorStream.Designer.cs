namespace RobotSensorStream
{
    partial class frmSensorStream
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea11 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series23 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title11 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea12 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series24 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title12 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea13 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend7 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series25 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series26 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series27 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title13 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea14 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend8 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series28 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series29 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series30 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title14 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea15 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend9 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series31 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series32 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series33 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title15 = new System.Windows.Forms.DataVisualization.Charting.Title();
            this.chtTemperature = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chtPressure = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chtAcceleration = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chtOrientation = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chtDirection = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.txtLog = new System.Windows.Forms.TextBox();
            this.cmbPort = new System.Windows.Forms.ComboBox();
            this.spSerialPort = new System.IO.Ports.SerialPort(this.components);
            this.btnConnect = new System.Windows.Forms.Button();
            this.tmrUpdate = new System.Windows.Forms.Timer(this.components);
            this.btnRefreshPorts = new System.Windows.Forms.Button();
            this.llblFourWalledCubicle = new System.Windows.Forms.LinkLabel();
            this.llblLUFALib = new System.Windows.Forms.LinkLabel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.pbLUFA = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.chtTemperature)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtPressure)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtAcceleration)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtOrientation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtDirection)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbLUFA)).BeginInit();
            this.SuspendLayout();
            // 
            // chtTemperature
            // 
            this.chtTemperature.BorderlineColor = System.Drawing.Color.Black;
            this.chtTemperature.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
            chartArea11.AxisX.Interval = 1D;
            chartArea11.AxisX.LabelStyle.Enabled = false;
            chartArea11.AxisX.Title = "Time";
            chartArea11.AxisY.Maximum = 60D;
            chartArea11.AxisY.Minimum = 0D;
            chartArea11.AxisY.Title = "Temperature (Deg C)";
            chartArea11.Name = "chaTempData";
            this.chtTemperature.ChartAreas.Add(chartArea11);
            this.chtTemperature.Location = new System.Drawing.Point(403, 230);
            this.chtTemperature.Name = "chtTemperature";
            series23.BorderWidth = 3;
            series23.ChartArea = "chaTempData";
            series23.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.SplineArea;
            series23.Name = "Temperature";
            this.chtTemperature.Series.Add(series23);
            this.chtTemperature.Size = new System.Drawing.Size(385, 212);
            this.chtTemperature.TabIndex = 0;
            this.chtTemperature.Text = "Temperature";
            title11.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title11.Name = "chtTemperature";
            title11.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title11.ShadowOffset = 1;
            title11.Text = "Temperature";
            this.chtTemperature.Titles.Add(title11);
            // 
            // chtPressure
            // 
            this.chtPressure.BorderlineColor = System.Drawing.Color.Black;
            this.chtPressure.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
            chartArea12.AxisX.Interval = 1D;
            chartArea12.AxisX.LabelStyle.Enabled = false;
            chartArea12.AxisX.Title = "Time";
            chartArea12.Name = "chaPressureData";
            this.chtPressure.ChartAreas.Add(chartArea12);
            this.chtPressure.Location = new System.Drawing.Point(12, 230);
            this.chtPressure.Name = "chtPressure";
            series24.BorderWidth = 3;
            series24.ChartArea = "chaPressureData";
            series24.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.SplineArea;
            series24.Name = "Temperature";
            series24.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            series24.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            this.chtPressure.Series.Add(series24);
            this.chtPressure.Size = new System.Drawing.Size(385, 212);
            this.chtPressure.TabIndex = 1;
            this.chtPressure.Text = "Pressure";
            title12.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title12.Name = "chtPressure";
            title12.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title12.ShadowOffset = 1;
            title12.Text = "Pressure";
            this.chtPressure.Titles.Add(title12);
            // 
            // chtAcceleration
            // 
            this.chtAcceleration.BorderlineColor = System.Drawing.Color.Black;
            this.chtAcceleration.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
            chartArea13.AxisX.Interval = 1D;
            chartArea13.AxisX.LabelStyle.Enabled = false;
            chartArea13.AxisX.Title = "Time";
            chartArea13.AxisY.Maximum = 2D;
            chartArea13.AxisY.Minimum = -2D;
            chartArea13.AxisY.Title = "Acceleration (g)";
            chartArea13.Name = "chaAccelData";
            this.chtAcceleration.ChartAreas.Add(chartArea13);
            legend7.Name = "Legend1";
            this.chtAcceleration.Legends.Add(legend7);
            this.chtAcceleration.Location = new System.Drawing.Point(12, 12);
            this.chtAcceleration.Name = "chtAcceleration";
            series25.BorderWidth = 3;
            series25.ChartArea = "chaAccelData";
            series25.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series25.Legend = "Legend1";
            series25.Name = "X";
            series26.BorderWidth = 3;
            series26.ChartArea = "chaAccelData";
            series26.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series26.Legend = "Legend1";
            series26.Name = "Y";
            series27.BorderWidth = 3;
            series27.ChartArea = "chaAccelData";
            series27.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series27.Legend = "Legend1";
            series27.Name = "Z";
            this.chtAcceleration.Series.Add(series25);
            this.chtAcceleration.Series.Add(series26);
            this.chtAcceleration.Series.Add(series27);
            this.chtAcceleration.Size = new System.Drawing.Size(385, 212);
            this.chtAcceleration.TabIndex = 2;
            this.chtAcceleration.Text = "Acceleration";
            title13.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title13.Name = "chtAcceleration";
            title13.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title13.ShadowOffset = 1;
            title13.Text = "Acceleration";
            this.chtAcceleration.Titles.Add(title13);
            // 
            // chtOrientation
            // 
            this.chtOrientation.BorderlineColor = System.Drawing.Color.Black;
            this.chtOrientation.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
            chartArea14.AxisX.Interval = 1D;
            chartArea14.AxisX.LabelStyle.Enabled = false;
            chartArea14.AxisX.Title = "Time";
            chartArea14.AxisY.Maximum = 180D;
            chartArea14.AxisY.Minimum = -180D;
            chartArea14.AxisY.Title = "Angle (Deg)";
            chartArea14.Name = "chaOrientationData";
            this.chtOrientation.ChartAreas.Add(chartArea14);
            legend8.Name = "Legend1";
            this.chtOrientation.Legends.Add(legend8);
            this.chtOrientation.Location = new System.Drawing.Point(403, 12);
            this.chtOrientation.Name = "chtOrientation";
            series28.BorderWidth = 3;
            series28.ChartArea = "chaOrientationData";
            series28.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series28.Legend = "Legend1";
            series28.Name = "X";
            series29.BorderWidth = 3;
            series29.ChartArea = "chaOrientationData";
            series29.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series29.Legend = "Legend1";
            series29.Name = "Y";
            series30.BorderWidth = 3;
            series30.ChartArea = "chaOrientationData";
            series30.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series30.Legend = "Legend1";
            series30.Name = "Z";
            this.chtOrientation.Series.Add(series28);
            this.chtOrientation.Series.Add(series29);
            this.chtOrientation.Series.Add(series30);
            this.chtOrientation.Size = new System.Drawing.Size(385, 212);
            this.chtOrientation.TabIndex = 3;
            this.chtOrientation.Text = "Orientation";
            title14.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title14.Name = "chtOrientation";
            title14.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title14.ShadowOffset = 1;
            title14.Text = "Orientation";
            this.chtOrientation.Titles.Add(title14);
            // 
            // chtDirection
            // 
            this.chtDirection.BorderlineColor = System.Drawing.Color.Black;
            this.chtDirection.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Dash;
            chartArea15.AxisX.Interval = 1D;
            chartArea15.AxisX.LabelStyle.Enabled = false;
            chartArea15.AxisX.Title = "Time";
            chartArea15.AxisY.Maximum = 1229D;
            chartArea15.AxisY.Minimum = -1229D;
            chartArea15.AxisY.Title = "Magnetic Flux (uT)";
            chartArea15.Name = "chaDirectionData";
            this.chtDirection.ChartAreas.Add(chartArea15);
            legend9.Name = "Legend1";
            this.chtDirection.Legends.Add(legend9);
            this.chtDirection.Location = new System.Drawing.Point(794, 12);
            this.chtDirection.Name = "chtDirection";
            series31.BorderWidth = 3;
            series31.ChartArea = "chaDirectionData";
            series31.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series31.Legend = "Legend1";
            series31.Name = "X";
            series32.BorderWidth = 3;
            series32.ChartArea = "chaDirectionData";
            series32.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series32.Legend = "Legend1";
            series32.Name = "Y";
            series33.BorderWidth = 3;
            series33.ChartArea = "chaDirectionData";
            series33.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series33.Legend = "Legend1";
            series33.Name = "Z";
            this.chtDirection.Series.Add(series31);
            this.chtDirection.Series.Add(series32);
            this.chtDirection.Series.Add(series33);
            this.chtDirection.Size = new System.Drawing.Size(385, 212);
            this.chtDirection.TabIndex = 4;
            this.chtDirection.Text = "Direction";
            title15.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title15.Name = "chtDirection";
            title15.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title15.ShadowOffset = 1;
            title15.Text = "Direction";
            this.chtDirection.Titles.Add(title15);
            // 
            // txtLog
            // 
            this.txtLog.Location = new System.Drawing.Point(796, 231);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.Size = new System.Drawing.Size(382, 210);
            this.txtLog.TabIndex = 5;
            // 
            // cmbPort
            // 
            this.cmbPort.FormattingEnabled = true;
            this.cmbPort.Location = new System.Drawing.Point(453, 488);
            this.cmbPort.Name = "cmbPort";
            this.cmbPort.Size = new System.Drawing.Size(136, 21);
            this.cmbPort.TabIndex = 6;
            this.cmbPort.SelectedIndexChanged += new System.EventHandler(this.cmbPort_SelectedIndexChanged);
            this.cmbPort.TextChanged += new System.EventHandler(this.cmbPort_TextChanged);
            // 
            // spSerialPort
            // 
            this.spSerialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.spSerialPort_DataReceived);
            // 
            // btnConnect
            // 
            this.btnConnect.Enabled = false;
            this.btnConnect.Location = new System.Drawing.Point(595, 485);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(123, 27);
            this.btnConnect.TabIndex = 7;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // tmrUpdate
            // 
            this.tmrUpdate.Enabled = true;
            this.tmrUpdate.Interval = 10;
            this.tmrUpdate.Tick += new System.EventHandler(this.tmrUpdate_Tick);
            // 
            // btnRefreshPorts
            // 
            this.btnRefreshPorts.Location = new System.Drawing.Point(595, 452);
            this.btnRefreshPorts.Name = "btnRefreshPorts";
            this.btnRefreshPorts.Size = new System.Drawing.Size(123, 27);
            this.btnRefreshPorts.TabIndex = 8;
            this.btnRefreshPorts.Text = "Refresh COM List";
            this.btnRefreshPorts.UseVisualStyleBackColor = true;
            this.btnRefreshPorts.Click += new System.EventHandler(this.btnRefreshPorts_Click);
            // 
            // llblFourWalledCubicle
            // 
            this.llblFourWalledCubicle.AutoSize = true;
            this.llblFourWalledCubicle.Location = new System.Drawing.Point(12, 502);
            this.llblFourWalledCubicle.Name = "llblFourWalledCubicle";
            this.llblFourWalledCubicle.Size = new System.Drawing.Size(144, 13);
            this.llblFourWalledCubicle.TabIndex = 9;
            this.llblFourWalledCubicle.TabStop = true;
            this.llblFourWalledCubicle.Text = "www.fourwalledcubicle.com/";
            this.llblFourWalledCubicle.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.llblFourWalledCubicle_LinkClicked);
            // 
            // llblLUFALib
            // 
            this.llblLUFALib.AutoSize = true;
            this.llblLUFALib.Location = new System.Drawing.Point(1096, 502);
            this.llblLUFALib.Name = "llblLUFALib";
            this.llblLUFALib.Size = new System.Drawing.Size(82, 13);
            this.llblLUFALib.TabIndex = 10;
            this.llblLUFALib.TabStop = true;
            this.llblLUFALib.Text = "www.lufa-lib.org";
            this.llblLUFALib.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.llblLUFALib_LinkClicked);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::RobotSensorStream.Properties.Resources.LUFA_thumb;
            this.pictureBox1.Location = new System.Drawing.Point(1099, 447);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(79, 51);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 12;
            this.pictureBox1.TabStop = false;
            // 
            // pbLUFA
            // 
            this.pbLUFA.Image = global::RobotSensorStream.Properties.Resources.FourWalledCubicle;
            this.pbLUFA.Location = new System.Drawing.Point(15, 448);
            this.pbLUFA.Name = "pbLUFA";
            this.pbLUFA.Size = new System.Drawing.Size(141, 51);
            this.pbLUFA.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pbLUFA.TabIndex = 11;
            this.pbLUFA.TabStop = false;
            // 
            // frmSensorStream
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1192, 524);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.pbLUFA);
            this.Controls.Add(this.llblLUFALib);
            this.Controls.Add(this.llblFourWalledCubicle);
            this.Controls.Add(this.btnRefreshPorts);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.cmbPort);
            this.Controls.Add(this.txtLog);
            this.Controls.Add(this.chtDirection);
            this.Controls.Add(this.chtOrientation);
            this.Controls.Add(this.chtAcceleration);
            this.Controls.Add(this.chtPressure);
            this.Controls.Add(this.chtTemperature);
            this.Name = "frmSensorStream";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Explorer Bot Sensor Streamer";
            this.Load += new System.EventHandler(this.frmSensorStream_Load);
            ((System.ComponentModel.ISupportInitialize)(this.chtTemperature)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtPressure)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtAcceleration)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtOrientation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtDirection)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbLUFA)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataVisualization.Charting.Chart chtTemperature;
        private System.Windows.Forms.DataVisualization.Charting.Chart chtPressure;
        private System.Windows.Forms.DataVisualization.Charting.Chart chtAcceleration;
        private System.Windows.Forms.DataVisualization.Charting.Chart chtOrientation;
        private System.Windows.Forms.DataVisualization.Charting.Chart chtDirection;
        private System.Windows.Forms.TextBox txtLog;
        private System.Windows.Forms.ComboBox cmbPort;
        private System.IO.Ports.SerialPort spSerialPort;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Timer tmrUpdate;
        private System.Windows.Forms.Button btnRefreshPorts;
        private System.Windows.Forms.LinkLabel llblFourWalledCubicle;
        private System.Windows.Forms.LinkLabel llblLUFALib;
        private System.Windows.Forms.PictureBox pbLUFA;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}

