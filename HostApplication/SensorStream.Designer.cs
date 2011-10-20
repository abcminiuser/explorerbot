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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea6 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series12 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title6 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea7 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series13 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title7 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea8 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend4 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series14 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series15 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series16 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title8 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea9 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend5 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series17 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series18 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series19 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title9 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea10 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend6 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series20 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series21 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series22 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title10 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmSensorStream));
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
            this.llblFourWalledCubicle = new System.Windows.Forms.LinkLabel();
            this.llblLUFALib = new System.Windows.Forms.LinkLabel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.pbLUFA = new System.Windows.Forms.PictureBox();
            this.btnRefreshPorts = new System.Windows.Forms.Button();
            this.pnlUIPositioner = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.chtTemperature)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtPressure)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtAcceleration)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtOrientation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtDirection)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbLUFA)).BeginInit();
            this.pnlUIPositioner.SuspendLayout();
            this.SuspendLayout();
            // 
            // chtTemperature
            // 
            this.chtTemperature.BorderlineColor = System.Drawing.Color.Black;
            this.chtTemperature.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea6.AxisX.Interval = 1D;
            chartArea6.AxisX.LabelStyle.Enabled = false;
            chartArea6.AxisX.Title = "Time";
            chartArea6.AxisY.Maximum = 60D;
            chartArea6.AxisY.Minimum = 0D;
            chartArea6.AxisY.Title = "Temperature (Deg C)";
            chartArea6.Name = "chaTempData";
            this.chtTemperature.ChartAreas.Add(chartArea6);
            this.chtTemperature.Location = new System.Drawing.Point(403, 225);
            this.chtTemperature.Name = "chtTemperature";
            series12.BorderWidth = 3;
            series12.ChartArea = "chaTempData";
            series12.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.SplineArea;
            series12.Name = "Temperature";
            this.chtTemperature.Series.Add(series12);
            this.chtTemperature.Size = new System.Drawing.Size(385, 212);
            this.chtTemperature.TabIndex = 0;
            this.chtTemperature.Text = "Temperature";
            title6.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title6.Name = "chtTemperature";
            title6.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title6.ShadowOffset = 1;
            title6.Text = "Temperature";
            this.chtTemperature.Titles.Add(title6);
            // 
            // chtPressure
            // 
            this.chtPressure.BorderlineColor = System.Drawing.Color.Black;
            this.chtPressure.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea7.AxisX.Interval = 1D;
            chartArea7.AxisX.LabelStyle.Enabled = false;
            chartArea7.AxisX.Title = "Time";
            chartArea7.AxisY.LabelStyle.Enabled = false;
            chartArea7.Name = "chaPressureData";
            this.chtPressure.ChartAreas.Add(chartArea7);
            this.chtPressure.Location = new System.Drawing.Point(12, 225);
            this.chtPressure.Name = "chtPressure";
            series13.BorderWidth = 3;
            series13.ChartArea = "chaPressureData";
            series13.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.SplineArea;
            series13.Name = "Temperature";
            series13.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            series13.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            this.chtPressure.Series.Add(series13);
            this.chtPressure.Size = new System.Drawing.Size(385, 212);
            this.chtPressure.TabIndex = 1;
            this.chtPressure.Text = "Pressure";
            title7.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title7.Name = "chtPressure";
            title7.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title7.ShadowOffset = 1;
            title7.Text = "Pressure";
            this.chtPressure.Titles.Add(title7);
            // 
            // chtAcceleration
            // 
            this.chtAcceleration.BorderlineColor = System.Drawing.Color.Black;
            this.chtAcceleration.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea8.AxisX.Interval = 1D;
            chartArea8.AxisX.LabelStyle.Enabled = false;
            chartArea8.AxisX.Title = "Time";
            chartArea8.AxisY.Maximum = 1.3D;
            chartArea8.AxisY.Minimum = -1.3D;
            chartArea8.AxisY.Title = "Acceleration (g)";
            chartArea8.Name = "chaAccelData";
            this.chtAcceleration.ChartAreas.Add(chartArea8);
            legend4.Name = "Legend1";
            this.chtAcceleration.Legends.Add(legend4);
            this.chtAcceleration.Location = new System.Drawing.Point(12, 7);
            this.chtAcceleration.Name = "chtAcceleration";
            series14.BorderWidth = 3;
            series14.ChartArea = "chaAccelData";
            series14.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series14.Legend = "Legend1";
            series14.Name = "X";
            series15.BorderWidth = 3;
            series15.ChartArea = "chaAccelData";
            series15.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series15.Legend = "Legend1";
            series15.Name = "Y";
            series16.BorderWidth = 3;
            series16.ChartArea = "chaAccelData";
            series16.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series16.Legend = "Legend1";
            series16.Name = "Z";
            this.chtAcceleration.Series.Add(series14);
            this.chtAcceleration.Series.Add(series15);
            this.chtAcceleration.Series.Add(series16);
            this.chtAcceleration.Size = new System.Drawing.Size(385, 212);
            this.chtAcceleration.TabIndex = 2;
            this.chtAcceleration.Text = "Acceleration";
            title8.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title8.Name = "chtAcceleration";
            title8.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title8.ShadowOffset = 1;
            title8.Text = "Acceleration";
            this.chtAcceleration.Titles.Add(title8);
            // 
            // chtOrientation
            // 
            this.chtOrientation.BorderlineColor = System.Drawing.Color.Black;
            this.chtOrientation.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea9.AxisX.Interval = 1D;
            chartArea9.AxisX.LabelStyle.Enabled = false;
            chartArea9.AxisX.Title = "Time";
            chartArea9.AxisY.Maximum = 150D;
            chartArea9.AxisY.Minimum = -150D;
            chartArea9.AxisY.Title = "Angle Change (Deg/Sec)";
            chartArea9.Name = "chaOrientationData";
            this.chtOrientation.ChartAreas.Add(chartArea9);
            legend5.Name = "Legend1";
            this.chtOrientation.Legends.Add(legend5);
            this.chtOrientation.Location = new System.Drawing.Point(403, 7);
            this.chtOrientation.Name = "chtOrientation";
            series17.BorderWidth = 3;
            series17.ChartArea = "chaOrientationData";
            series17.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series17.Legend = "Legend1";
            series17.Name = "X";
            series18.BorderWidth = 3;
            series18.ChartArea = "chaOrientationData";
            series18.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series18.Legend = "Legend1";
            series18.Name = "Y";
            series19.BorderWidth = 3;
            series19.ChartArea = "chaOrientationData";
            series19.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series19.Legend = "Legend1";
            series19.Name = "Z";
            this.chtOrientation.Series.Add(series17);
            this.chtOrientation.Series.Add(series18);
            this.chtOrientation.Series.Add(series19);
            this.chtOrientation.Size = new System.Drawing.Size(385, 212);
            this.chtOrientation.TabIndex = 3;
            this.chtOrientation.Text = "Rotation";
            title9.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title9.Name = "chtOrientation";
            title9.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title9.ShadowOffset = 1;
            title9.Text = "Orientation";
            this.chtOrientation.Titles.Add(title9);
            // 
            // chtDirection
            // 
            this.chtDirection.BorderlineColor = System.Drawing.Color.Black;
            this.chtDirection.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea10.AxisX.Interval = 1D;
            chartArea10.AxisX.LabelStyle.Enabled = false;
            chartArea10.AxisX.Title = "Time";
            chartArea10.AxisY.Maximum = 150D;
            chartArea10.AxisY.Minimum = -150D;
            chartArea10.AxisY.Title = "Magnetic Flux (uT)";
            chartArea10.Name = "chaDirectionData";
            this.chtDirection.ChartAreas.Add(chartArea10);
            legend6.Name = "Legend1";
            this.chtDirection.Legends.Add(legend6);
            this.chtDirection.Location = new System.Drawing.Point(794, 7);
            this.chtDirection.Name = "chtDirection";
            series20.BorderWidth = 3;
            series20.ChartArea = "chaDirectionData";
            series20.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series20.Legend = "Legend1";
            series20.Name = "X";
            series21.BorderWidth = 3;
            series21.ChartArea = "chaDirectionData";
            series21.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series21.Legend = "Legend1";
            series21.Name = "Y";
            series22.BorderWidth = 3;
            series22.ChartArea = "chaDirectionData";
            series22.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series22.Legend = "Legend1";
            series22.Name = "Z";
            this.chtDirection.Series.Add(series20);
            this.chtDirection.Series.Add(series21);
            this.chtDirection.Series.Add(series22);
            this.chtDirection.Size = new System.Drawing.Size(385, 212);
            this.chtDirection.TabIndex = 4;
            this.chtDirection.Text = "Direction";
            title10.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title10.Name = "chtDirection";
            title10.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title10.ShadowOffset = 1;
            title10.Text = "Direction";
            this.chtDirection.Titles.Add(title10);
            // 
            // txtLog
            // 
            this.txtLog.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtLog.Location = new System.Drawing.Point(794, 225);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.Size = new System.Drawing.Size(385, 212);
            this.txtLog.TabIndex = 5;
            // 
            // cmbPort
            // 
            this.cmbPort.FormattingEnabled = true;
            this.cmbPort.Location = new System.Drawing.Point(487, 473);
            this.cmbPort.Name = "cmbPort";
            this.cmbPort.Size = new System.Drawing.Size(136, 21);
            this.cmbPort.TabIndex = 6;
            this.cmbPort.SelectedIndexChanged += new System.EventHandler(this.cmbPort_SelectedIndexChanged);
            this.cmbPort.TextChanged += new System.EventHandler(this.cmbPort_TextChanged);
            // 
            // spSerialPort
            // 
            this.spSerialPort.DtrEnable = true;
            this.spSerialPort.RtsEnable = true;
            this.spSerialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.spSerialPort_DataReceived);
            // 
            // btnConnect
            // 
            this.btnConnect.Enabled = false;
            this.btnConnect.Location = new System.Drawing.Point(629, 473);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(123, 21);
            this.btnConnect.TabIndex = 7;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // tmrUpdate
            // 
            this.tmrUpdate.Enabled = true;
            this.tmrUpdate.Interval = 5;
            this.tmrUpdate.Tick += new System.EventHandler(this.tmrUpdate_Tick);
            // 
            // llblFourWalledCubicle
            // 
            this.llblFourWalledCubicle.Location = new System.Drawing.Point(12, 497);
            this.llblFourWalledCubicle.Name = "llblFourWalledCubicle";
            this.llblFourWalledCubicle.Size = new System.Drawing.Size(141, 13);
            this.llblFourWalledCubicle.TabIndex = 9;
            this.llblFourWalledCubicle.TabStop = true;
            this.llblFourWalledCubicle.Text = "www.fourwalledcubicle.com";
            this.llblFourWalledCubicle.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.llblFourWalledCubicle.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.llblFourWalledCubicle_LinkClicked);
            // 
            // llblLUFALib
            // 
            this.llblLUFALib.Location = new System.Drawing.Point(1034, 497);
            this.llblLUFALib.Name = "llblLUFALib";
            this.llblLUFALib.Size = new System.Drawing.Size(144, 13);
            this.llblLUFALib.TabIndex = 10;
            this.llblLUFALib.TabStop = true;
            this.llblLUFALib.Text = "www.lufa-lib.org";
            this.llblLUFALib.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.llblLUFALib.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.llblLUFALib_LinkClicked);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::RobotSensorStream.Properties.Resources.LUFA_thumb;
            this.pictureBox1.Location = new System.Drawing.Point(1037, 443);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(141, 51);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 12;
            this.pictureBox1.TabStop = false;
            // 
            // pbLUFA
            // 
            this.pbLUFA.Image = global::RobotSensorStream.Properties.Resources.FourWalledCubicle;
            this.pbLUFA.Location = new System.Drawing.Point(12, 443);
            this.pbLUFA.Name = "pbLUFA";
            this.pbLUFA.Size = new System.Drawing.Size(141, 51);
            this.pbLUFA.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pbLUFA.TabIndex = 11;
            this.pbLUFA.TabStop = false;
            // 
            // btnRefreshPorts
            // 
            this.btnRefreshPorts.Image = global::RobotSensorStream.Properties.Resources.action_refresh_blue;
            this.btnRefreshPorts.Location = new System.Drawing.Point(453, 473);
            this.btnRefreshPorts.Name = "btnRefreshPorts";
            this.btnRefreshPorts.Size = new System.Drawing.Size(28, 21);
            this.btnRefreshPorts.TabIndex = 8;
            this.btnRefreshPorts.UseVisualStyleBackColor = true;
            this.btnRefreshPorts.Click += new System.EventHandler(this.btnRefreshPorts_Click);
            // 
            // pnlUIPositioner
            // 
            this.pnlUIPositioner.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.pnlUIPositioner.Controls.Add(this.pictureBox1);
            this.pnlUIPositioner.Controls.Add(this.pbLUFA);
            this.pnlUIPositioner.Controls.Add(this.llblLUFALib);
            this.pnlUIPositioner.Controls.Add(this.llblFourWalledCubicle);
            this.pnlUIPositioner.Controls.Add(this.btnRefreshPorts);
            this.pnlUIPositioner.Controls.Add(this.btnConnect);
            this.pnlUIPositioner.Controls.Add(this.cmbPort);
            this.pnlUIPositioner.Controls.Add(this.txtLog);
            this.pnlUIPositioner.Controls.Add(this.chtDirection);
            this.pnlUIPositioner.Controls.Add(this.chtOrientation);
            this.pnlUIPositioner.Controls.Add(this.chtAcceleration);
            this.pnlUIPositioner.Controls.Add(this.chtPressure);
            this.pnlUIPositioner.Controls.Add(this.chtTemperature);
            this.pnlUIPositioner.Location = new System.Drawing.Point(0, 5);
            this.pnlUIPositioner.Name = "pnlUIPositioner";
            this.pnlUIPositioner.Size = new System.Drawing.Size(1190, 518);
            this.pnlUIPositioner.TabIndex = 13;
            // 
            // frmSensorStream
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1192, 524);
            this.Controls.Add(this.pnlUIPositioner);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
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
            this.pnlUIPositioner.ResumeLayout(false);
            this.pnlUIPositioner.PerformLayout();
            this.ResumeLayout(false);

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
        private System.Windows.Forms.Panel pnlUIPositioner;
    }
}

