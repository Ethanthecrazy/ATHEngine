using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class AddProperty : Form
    {
        private bool editing;
        private string originalName;
        private string originalValue;

        public AddProperty(bool _editing = false, string _originalName = "", string _originalValue = "")
        {
            InitializeComponent();
            editing = _editing;
            originalName = _originalName;
            originalValue = _originalValue;
        }

        private void AddProperty_Load(object sender, EventArgs e)
        {
            MainForm form = (MainForm)this.Owner;

            foreach (cObject obj in form.ObjectManager.ObjectList)
            {
                foreach (Property p in obj.Properties)
                {
                    if (!comboBox_Property.Items.Contains(p.Name))
                        comboBox_Property.Items.Add(p.Name);
                }
            }

            if (editing)
            {
                comboBox_Property.Text = originalName;
                textBox2.Text = originalValue;
                this.Text = "Edit Property";
                form.ObjectManager.SelectedObject.Properties.Remove(new Property(originalName, originalValue));
            }


            Point loc = form.Location;
            loc.X += form.mainPanel.Panel1.Size.Width - this.Size.Width + 8;
            loc.Y += form.mainPanel.Panel1.Size.Height - 25;
            

            this.Location = loc;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            MainForm form = (MainForm)this.Owner;

            foreach (Property p in form.ObjectManager.SelectedObject.Properties)
            {
                if (p.Name == comboBox_Property.Text)
                {
                    if (DialogResult.OK == MessageBox.Show("ERROR: Can't add a property that an object already owns.", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error)) 
                        return;                    
                }
            }

            form.ObjectManager.SelectedObject.Properties.Add(new Property(comboBox_Property.Text, textBox2.Text));

            this.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            
        }

        private void AddProperty_FormClosed(object sender, FormClosedEventArgs e)
        {            
            MainForm form = (MainForm)this.Owner;
            
            if (editing && this.DialogResult == System.Windows.Forms.DialogResult.Cancel)
                form.ObjectManager.SelectedObject.Properties.Add(new Property(originalName, originalValue));

            form.Form_AddProperty = null;    
        }


    }
}
