using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing.Imaging;
using System.Xml;


namespace WindowsFormsApplication1
{
    //OPTIONS: Adjust font size of list box
    //EXTRAS:
    // ability to give layer number to the objects for rendering and selecting priority

    //NOTE: any references to "Form1" such as "Form1_Resize" is referring to the MainForm

    public partial class MainForm : Form
    {
        public static string VERSION = "1.0.0.0";

        private Point lastMouse = new Point();
        private bool isConvex = true;
        private int greatestWidth = 0;
        private int greatestHeight = 0;
        private float zoomLevel = 1.0f;
        private float pointSize = 16.0f;

        private enum Saved_State { NOTSAVED = 0, SAVED, NEVERSAVED };
        private Saved_State saved = Saved_State.NEVERSAVED;
        private string fileName = "";
        
        private ObjectManager objectManager = new ObjectManager();
        public ObjectManager ObjectManager
        {
            get { return objectManager; }
            set { objectManager = value; }
        }

        private AddProperty form_AddProperty = null;
        public AddProperty Form_AddProperty
        {
            get { return form_AddProperty; }
            set { form_AddProperty = value; }
        }

        public SplitContainer mainPanel;

        public static void SetDoubleBuffered(System.Windows.Forms.Control c)
        {
            if (System.Windows.Forms.SystemInformation.TerminalServerSession)
                return;

            System.Reflection.PropertyInfo aProp =
                  typeof(System.Windows.Forms.Control).GetProperty(
                        "DoubleBuffered",
                        System.Reflection.BindingFlags.NonPublic |
                        System.Reflection.BindingFlags.Instance);

            aProp.SetValue(c, true, null);
        }

        public MainForm()
        {
            InitializeComponent();

            SetDoubleBuffered(panel1);
            SetDoubleBuffered(splitContainer1.Panel1);
            SetDoubleBuffered(splitContainer1.Panel2);

            splitContainer1_SplitterMoved(null, null);
            ButtonsEnabled(false);

            mainPanel = splitContainer1;
        }
        
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (saved == Saved_State.NOTSAVED || (saved == Saved_State.NEVERSAVED && objectManager.ObjectList.Count > 0))
            {
                DialogResult result = MessageBox.Show("You have unsaved changes, you will loose any unsaved progress. Would you like to continue?",
                    "Confirmation", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);

                if (result == System.Windows.Forms.DialogResult.No)
                {
                    return;
                }
            }

            objectManager.ObjectList.Clear();

            saved = Saved_State.NEVERSAVED;
            splitContainer1.Panel1.AutoScrollMinSize = new Size(0, 0);
            objectManager.SelectedObject = null;
            ButtonsEnabled(false);
            Invalidate();
        }

        /// <summary>
        /// Creates a relative path from one file or folder to another.
        /// </summary>
        /// <param name="fromPath">Contains the directory that defines the start of the relative path.</param>
        /// <param name="toPath">Contains the path that defines the endpoint of the relative path.</param>
        /// <returns>The relative path from the start directory to the end path.</returns>
        /// <exception cref="ArgumentNullException"></exception>
        public static String MakeRelativePath(String fromPath, String toPath)
        {
            if (String.IsNullOrEmpty(fromPath)) throw new ArgumentNullException("fromPath");
            if (String.IsNullOrEmpty(toPath)) throw new ArgumentNullException("toPath");

            Uri fromUri = new Uri(fromPath);
            Uri toUri = new Uri(toPath);

            Uri relativeUri = fromUri.MakeRelativeUri(toUri);
            String relativePath = Uri.UnescapeDataString(relativeUri.ToString());

            return relativePath.Replace('/', Path.DirectorySeparatorChar);
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "Image Files (*.bmp, *.gif, *.jpeg, *.jpg, *.png, *.tiff)|*.bmp;*.gif;*.jpeg;*.jpg;*.png;*.tiff|XML Files (*.xml)|*.xml";
            ofd.Multiselect = true;

            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                if (ofd.FilterIndex == 1)
                {
                    foreach (string s in ofd.FileNames)
                    {
                        string relativePath = MakeRelativePath(Application.ExecutablePath, s);

                        cObject t_object = new cObject(new Bitmap(s), relativePath);

                        objectManager.AddObject(t_object);
                        saved = Saved_State.NOTSAVED;
                    }
                }
                else
                {
                    XmlDocument xmlDoc = new XmlDocument(); // Create an XML document object
                    int objOpend = 0;
                    int wrldOpend = 0;

                    foreach (string s in ofd.FileNames)
                    {
                        xmlDoc.Load(s); // Load the XML document from the specified file
                        bool exportedObject = false;

                        XmlNode versionNode = xmlDoc.SelectSingleNode("/World");
                        if (null == versionNode)
                        {
                            versionNode = xmlDoc.SelectSingleNode("/Exported_Object");
                            if (null == versionNode)
                            {
                                MessageBox.Show("ERROR: Cannot find root \"World\" or \"Exported_Object\".", "ERROR: Bad File", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                return;
                            }
                            else
                                exportedObject = true;
                        }

                        if (null == versionNode.Attributes["Version"])
                        {
                            MessageBox.Show("ERROR: XML file is does not contain version number.", "ERROR: Bad Version", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return;
                        }
                        string worldVersion = versionNode.Attributes["Version"].Value;
                        
                        
                        if (0 != String.Compare(worldVersion, VERSION))
                        {
                            MessageBox.Show("ERROR: XML file is using a version different from the Editor.", "ERROR: Old Version", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            return;
                        }


                        if (!exportedObject)
                        {
                            XmlNode objectsNode = xmlDoc.SelectSingleNode("/World/Objects");
                            int numOfObjects = int.Parse(objectsNode.Attributes["Number_of_Objects"].Value);


                            XmlNodeList objectChildren = objectsNode.ChildNodes;
                            XmlNode objectNode;
                            for (int i = 0; i < numOfObjects; ++i)
                            {
                                objectNode = objectChildren[i];

                                if (null != objectNode.Attributes["Image_Path"])
                                {
                                    string path = objectNode.Attributes["Image_Path"].Value;
                                    path = Path.GetFullPath(path);
                                    string relativePath = MakeRelativePath(Application.ExecutablePath, path);

                                    cObject t_object = new cObject(new Bitmap(path), relativePath);

                                    t_object.LoadXML(objectNode);

                                    objectManager.AddObject(t_object);
                                    ++objOpend;
                                }
                                else
                                    MessageBox.Show("ERROR: XML file has an object with a bad image file path.", "ERROR: Bad File Path", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            }

                            ++wrldOpend;
                        }
                        else
                        {
                            XmlNode objectNode = xmlDoc.SelectSingleNode("/Exported_Object/Object");

                            if (null != objectNode.Attributes["Image_Path"])
                            {
                                string path = objectNode.Attributes["Image_Path"].Value;
                                path = Path.GetFullPath(path);
                                string relativePath = MakeRelativePath(Application.ExecutablePath, path);

                                cObject t_object = new cObject(new Bitmap(path), relativePath);

                                t_object.LoadXML(objectNode);

                                objectManager.AddObject(t_object);
                                ++objOpend;

                                saved = Saved_State.NOTSAVED;
                            }
                            else
                                MessageBox.Show("ERROR: XML file has an object with a bad image file path.", "ERROR: Bad File Path", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }

                    }
                    
                    if (saved == Saved_State.NEVERSAVED && wrldOpend == 1)
                        saved = Saved_State.SAVED;
                    else
                        saved = Saved_State.NOTSAVED;
                }

                FindCanvasSize();
                objectManager.SelectedObject = null;
                ButtonsEnabled(false);                
                Invalidate();
            }
        }

        private bool CheckEverythingConvex()
        {
            foreach (cObject obj in objectManager.ObjectList)
            {
                if (obj.CollisionPoints.Count > 3 && !IsConvex(obj.CollisionPoints))
                {
                    if (MessageBox.Show("At least one object is not convex, would you like to save anyways?", 
                        "Confirmation", MessageBoxButtons.YesNo, MessageBoxIcon.Warning)
                        == System.Windows.Forms.DialogResult.Yes)
                    {
                        return true;
                    }

                    return false;
                }
            }

            return true;
        }

        private void SaveXML(string _fileName)
        {
            if (!CheckEverythingConvex())
                return;

            XmlTextWriter textWriter = new XmlTextWriter(_fileName, null);
            textWriter.WriteStartDocument();

            textWriter.WriteStartElement("World");
            textWriter.WriteAttributeString("Version", VERSION);


            textWriter.WriteStartElement("Objects");
            textWriter.WriteAttributeString("Number_of_Objects", objectManager.ObjectList.Count.ToString());
            foreach (cObject obj in objectManager.ObjectList)
            {
                obj.SaveXML(textWriter);
            }
            textWriter.WriteEndElement();

            textWriter.WriteEndDocument();
            textWriter.Close();

            fileName = _fileName;
            saved = Saved_State.SAVED;
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (saved == Saved_State.NEVERSAVED)
            {
                saveAsToolStripMenuItem_Click(sender, e);
                return;
            }
            else if (saved == Saved_State.NOTSAVED)
            {
                if (fileName == string.Empty)
                {
                    saveAsToolStripMenuItem_Click(sender, e);
                    return;
                }

                SaveXML(fileName);
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "XML Files (*.xml)|*.xml"; //|World Files (*.wld)|*.wld";

            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                if (sfd.FilterIndex == 1)
                {
                    // XML file
                    SaveXML(sfd.FileName);
                }
            }
        }

        private void Export_toolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "XML Files (*.xml)|*.xml"; //|World Files (*.wld)|*.wld";

            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                objectManager.SelectedObject.Export(sfd.FileName);
            }
        }


        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void propertiesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            splitContainer1.Panel2Collapsed = !splitContainer1.Panel2Collapsed;
            propertiesToolStripMenuItem.Checked = !propertiesToolStripMenuItem.Checked;
            Invalidate();
        }

        private void splitContainer1_Panel1_Paint(object sender, PaintEventArgs e)
        {
            Point offset = Point.Empty;

            offset.X += splitContainer1.Panel1.AutoScrollPosition.X;
            offset.Y += splitContainer1.Panel1.AutoScrollPosition.Y;

            //RectangleF cameraRect = new RectangleF(offset.X * -1, offset.Y * -1, splitContainer1.Panel1.Width, splitContainer1.Panel1.Height);

            foreach (cObject obj in objectManager.ObjectList)
            {
                if (obj != objectManager.SelectedObject)
                {
                    //e.Graphics.DrawImage(obj.Image, obj.Rectangle);

                    e.Graphics.DrawImage(obj.Image,
                                            new RectangleF((obj.Position.X * zoomLevel + offset.X),
                                                (obj.Position.Y * zoomLevel + offset.Y) ,
                                                obj.Image.Width * zoomLevel,
                                                obj.Image.Height * zoomLevel),
                                            new RectangleF(0, 0,  obj.Image.Width,  obj.Image.Height),
                                            GraphicsUnit.Pixel);
                }
            }


            if (objectManager.SelectedObject != null)
            {
                //e.Graphics.DrawImage(objectManager.SelectedObject.Image, objectManager.SelectedObject.Rectangle);
                //e.Graphics.DrawRectangle(new Pen(Color.Black, 3), objectManager.SelectedObject.Rectangle);
                e.Graphics.DrawImage(objectManager.SelectedObject.Image,
                                            new RectangleF((objectManager.SelectedObject.Position.X * zoomLevel + offset.X) ,
                                                (objectManager.SelectedObject.Position.Y * zoomLevel + offset.Y) ,
                                                objectManager.SelectedObject.Image.Width * zoomLevel,
                                                objectManager.SelectedObject.Image.Height * zoomLevel),
                                            new RectangleF(0, 0, objectManager.SelectedObject.Image.Width, objectManager.SelectedObject.Image.Height),
                                            GraphicsUnit.Pixel);
                e.Graphics.DrawRectangle(new Pen(Color.Black, 3), (objectManager.SelectedObject.Position.X * zoomLevel + offset.X) ,
                                                (objectManager.SelectedObject.Position.Y * zoomLevel + offset.Y) ,
                                                objectManager.SelectedObject.Image.Width * zoomLevel,
                                                objectManager.SelectedObject.Image.Height * zoomLevel);

                if (collisionPolygonToolStripMenuItem.Checked)
                {
                    if (objectManager.SelectedObject.CollisionPoints.Count > 0)
                    {
                        if (objectManager.SelectedObject.CollisionPoints.Count > 1)
                        {
                            Pen pen = new Pen(Color.Red, 2);

                            PointF[] tempy = new PointF[objectManager.SelectedObject.CollisionPoints.Count];
                            objectManager.SelectedObject.CollisionPoints.CopyTo(tempy);
                            for(int i = 0; i < tempy.Count(); ++i)
                            {
                                tempy[i] = new PointF((tempy[i].X * zoomLevel + offset.X), (tempy[i].Y * zoomLevel + offset.Y));
                            }

                            e.Graphics.DrawLines(pen, tempy);

                            if (objectManager.SelectedObject.CollisionPoints.Count > 2)
                            {
                                e.Graphics.DrawLine(pen,
                                    new PointF(tempy[0].X, tempy[0].Y),
                                    new PointF(tempy[tempy.Count() - 1].X, tempy[tempy.Count() - 1].Y));


                                if (objectManager.SelectedObject.CollisionPoints.Count > 3)
                                    label_NotConvex.Visible = !isConvex;
                            }
                        }

                        foreach (PointF p in objectManager.SelectedObject.CollisionPoints)
                        {      
                            if (objectManager.SelectedObject.SelectedPoint != -1 && 
                                p == objectManager.SelectedObject.CollisionPoints[objectManager.SelectedObject.SelectedPoint])
                            {
                                e.Graphics.DrawEllipse(new Pen(Color.Blue, 4), (p.X * zoomLevel - (pointSize / 2) + offset.X), (p.Y * zoomLevel - (pointSize / 2) + offset.Y), pointSize, pointSize);
                                continue;
                            }
                            e.Graphics.FillEllipse(new SolidBrush(Color.Blue), (p.X * zoomLevel - (pointSize / 2) + offset.X), (p.Y * zoomLevel - (pointSize / 2) + offset.Y), pointSize, pointSize);
                        }
                    }
                }
            }
        }

        private new void Invalidate()
        {
            splitContainer1.Panel1.Invalidate();
        }

        private void ButtonsEnabled(bool value)
        {
            toolStripButton_AddProperty.Enabled = value;
            button_AddProperty.Enabled = value;
            button_RemoveProperty.Enabled = value;
            Export_toolStripMenuItem.Enabled = value;
        }


        private void splitContainer1_Panel1_MouseDown(object sender, MouseEventArgs e)
        {
            Point offset = Point.Empty;

            offset.X += splitContainer1.Panel1.AutoScrollPosition.X;
            offset.Y += splitContainer1.Panel1.AutoScrollPosition.Y;

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                if (null != objectManager.SelectedObject)
                {           
                    RectangleF rect = Rectangle.Empty;
                    for(int i = 0; i < objectManager.SelectedObject.CollisionPoints.Count; ++i)
                    {
                        rect = new RectangleF(
                            new PointF((objectManager.SelectedObject.CollisionPoints[i].X * zoomLevel - (pointSize / 2) + offset.X), (objectManager.SelectedObject.CollisionPoints[i].Y * zoomLevel - (pointSize / 2) + offset.Y)), 
                            new SizeF(pointSize, pointSize));

                        if (rect.Contains(e.Location))
                        {
                            objectManager.SelectedObject.SelectedPoint = i;
                            Invalidate();
                            return;
                        }
                    }

                    objectManager.SelectedObject.SelectedPoint = -1;
                    Invalidate();

                    if (new RectangleF(
                        new PointF((objectManager.SelectedObject.Position.X * zoomLevel + offset.X), (objectManager.SelectedObject.Position.Y * zoomLevel + offset.Y)),
                        new SizeF(objectManager.SelectedObject.Rectangle.Size.Width * zoomLevel, objectManager.SelectedObject.Rectangle.Size.Height * zoomLevel)).Contains(e.Location))
                        return;
                }

                objectManager.SelectedObject = null;

                if (objectManager.ObjectList.Count != 0)
                {
                    RectangleF rect = new Rectangle();
                    foreach (cObject obj in objectManager.ObjectList)
                    {
                        rect.Location = new PointF((obj.Position.X * zoomLevel + offset.X), (obj.Position.Y * zoomLevel + offset.Y));
                        rect.Size = new SizeF(obj.Image.Size.Width * zoomLevel, obj.Image.Size.Height * zoomLevel);

                        if (rect.Contains(e.Location))
                        {
                            objectManager.SelectedObject = obj;
                            break;
                        }
                    }
                }

                ButtonsEnabled(objectManager.SelectedObject != null);
                FixViewList();
                Invalidate();
            }
        }

        private void splitContainer1_Panel1_MouseMove(object sender, MouseEventArgs e)
        {
            if (objectManager.SelectedObject != null && e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                Point deltaMoved = new Point(e.Location.X - lastMouse.X, e.Location.Y - lastMouse.Y);

                if (objectManager.SelectedObject.SelectedPoint == -1)
                {
                    Point newPos = new Point(                       
                        objectManager.SelectedObject.Position.X + (int)(deltaMoved.X / zoomLevel),
                        objectManager.SelectedObject.Position.Y + (int)(deltaMoved.Y / zoomLevel));

                    // clip object to screen
                    if (newPos.X < 0)
                        newPos.X = 0;

                    if (newPos.Y < 0)
                        newPos.Y = 0;

                    // move collision points attached to this object
                    Point moved = new Point(newPos.X - objectManager.SelectedObject.Position.X, newPos.Y - objectManager.SelectedObject.Position.Y);
                    for (int i = 0; i < objectManager.SelectedObject.CollisionPoints.Count; ++i)
                    {
                        PointF curr = objectManager.SelectedObject.CollisionPoints[i];
                        objectManager.SelectedObject.CollisionPoints[i] = new PointF(curr.X + moved.X, curr.Y + moved.Y);
                    }                   

                    objectManager.SelectedObject.Position = newPos;

                    //extend canvas size if needed
                    FindCanvasSize();                    

                    //move the scrollbar based on if the object is on the edge
                    if ((objectManager.SelectedObject.Position.X + objectManager.SelectedObject.Image.Width) * zoomLevel >= splitContainer1.Panel1.Width * zoomLevel + -splitContainer1.Panel1.AutoScrollPosition.X && deltaMoved.X > 0)
                    {
                        splitContainer1.Panel1.AutoScrollPosition = new Point(-splitContainer1.Panel1.AutoScrollPosition.X + deltaMoved.X,
                                                                              -splitContainer1.Panel1.AutoScrollPosition.Y);
                    }
                    else if (objectManager.SelectedObject.Position.X <= -splitContainer1.Panel1.AutoScrollPosition.X && deltaMoved.X < 0)
                    {
                        splitContainer1.Panel1.AutoScrollPosition = new Point(-splitContainer1.Panel1.AutoScrollPosition.X + deltaMoved.X,
                                                                              -splitContainer1.Panel1.AutoScrollPosition.Y);
                    }

                    if ((objectManager.SelectedObject.Position.Y + objectManager.SelectedObject.Image.Height) * zoomLevel >= splitContainer1.Panel1.Height * zoomLevel + -splitContainer1.Panel1.AutoScrollPosition.Y && deltaMoved.Y > 0)
                    {
                        splitContainer1.Panel1.AutoScrollPosition = new Point(-splitContainer1.Panel1.AutoScrollPosition.X,
                                                                              -splitContainer1.Panel1.AutoScrollPosition.Y + deltaMoved.Y);
                    }
                    else if (objectManager.SelectedObject.Position.Y <= -splitContainer1.Panel1.AutoScrollPosition.Y && deltaMoved.Y < 0)
                    {
                        splitContainer1.Panel1.AutoScrollPosition = new Point(-splitContainer1.Panel1.AutoScrollPosition.X,
                                                                              -splitContainer1.Panel1.AutoScrollPosition.Y + deltaMoved.Y);
                    }
                }
                else
                {
                    //move the selected collision point
                    PointF newPos = new PointF(
                        objectManager.SelectedObject.CollisionPoints[objectManager.SelectedObject.SelectedPoint].X + (int)(deltaMoved.X / zoomLevel),
                        objectManager.SelectedObject.CollisionPoints[objectManager.SelectedObject.SelectedPoint].Y + (int)(deltaMoved.Y / zoomLevel));

                    //clamp the point to the edges of the screen (the hardcoded 30 is to handle not moving the point past the scrollbar)
                    if (newPos.X < 0 + -splitContainer1.Panel1.AutoScrollPosition.X)
                        newPos.X = 0 + -splitContainer1.Panel1.AutoScrollPosition.X;
                    else if ((newPos.X + (pointSize / 2)) * zoomLevel > (splitContainer1.Panel1.Width + -splitContainer1.Panel1.AutoScrollPosition.X) - 30 && deltaMoved.X > 0)
                        newPos.X = (splitContainer1.Panel1.Width + -splitContainer1.Panel1.AutoScrollPosition.X) / zoomLevel - (pointSize / 2) - (30 / zoomLevel);

                    if (newPos.Y < 0 + -splitContainer1.Panel1.AutoScrollPosition.Y)
                        newPos.Y = 0 + -splitContainer1.Panel1.AutoScrollPosition.Y;
                    else if ((newPos.Y + (pointSize / 2)) * zoomLevel > splitContainer1.Panel1.Height + -splitContainer1.Panel1.AutoScrollPosition.Y - 30 && deltaMoved.Y > 0)
                        newPos.Y = (splitContainer1.Panel1.Height - splitContainer1.Panel1.AutoScrollPosition.Y) / zoomLevel - (pointSize / 2) - (30 / zoomLevel);

                    objectManager.SelectedObject.CollisionPoints[objectManager.SelectedObject.SelectedPoint] = newPos;

                    //moving the point around so test whether the collision polygon is convex
                    isConvex = objectManager.SelectedObject.CollisionPoints.Count <= 3 ? true : IsConvex(objectManager.SelectedObject.CollisionPoints);
                }

                saved = Saved_State.NOTSAVED;
                Invalidate();
            }

            lastMouse = e.Location;
        }

        private void splitContainer1_SplitterMoved(object sender, SplitterEventArgs e)
        {
            button_AddProperty.Size = new Size(splitContainer2.Panel2.ClientSize.Width / 2, button_AddProperty.Size.Height);
            button_RemoveProperty.Size = new Size(splitContainer2.Panel2.ClientSize.Width / 2, button_RemoveProperty.Size.Height);

            listView_Properties.Columns[0].Width = splitContainer2.Panel1.ClientSize.Width / 2 - 4;
            listView_Properties.Columns[1].Width = splitContainer2.Panel1.ClientSize.Width / 2;
        }

        private void FixViewList()
        {
            listView_Properties.Items.Clear();
            if (null != objectManager.SelectedObject)
            {
                foreach (Property p in objectManager.SelectedObject.Properties)
                    listView_Properties.Items.Add(new ListViewItem(p.ToMyArray()));
            }
            listView_Properties.Invalidate();
        }

        private void button_AddProperty_Click(object sender, EventArgs e)
        {
            if (null != objectManager.SelectedObject && null == form_AddProperty)
            {
                form_AddProperty = new AddProperty();

                if (System.Windows.Forms.DialogResult.OK == form_AddProperty.ShowDialog(this))
                {
                    objectManager.SelectedObject.Properties.Sort(delegate(Property p1, Property p2) { return p1.Name.CompareTo(p2.Name); });
                    FixViewList();                                        
                }
            }
        }

        private void button_RemoveProperty_Click(object sender, EventArgs e)
        {
            if (null != objectManager.SelectedObject && listView_Properties.SelectedIndices.Count != 0)
            {
                List<Property> removeList = new List<Property>();
                for (int i = 0; i < listView_Properties.SelectedIndices.Count; ++i)
                    removeList.Add(objectManager.SelectedObject.Properties[listView_Properties.SelectedIndices[i]]);

                foreach (Property p in removeList)
                    objectManager.SelectedObject.Properties.Remove(p);

                FixViewList();
            }
        }

        private void listBox_Properties_DoubleClick(object sender, EventArgs e)
        {
            if (null != objectManager.SelectedObject && null == form_AddProperty)
            {
                Property temp = objectManager.SelectedObject.Properties[listView_Properties.SelectedIndices[0]];
                form_AddProperty = new AddProperty(true, temp.Name, temp.Value);

                form_AddProperty.ShowDialog(this);

                objectManager.SelectedObject.Properties.Sort(delegate(Property p1, Property p2) { return p1.Name.CompareTo(p2.Name); });
                FixViewList();
            }
        }

        private void listView_Properties_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                button_RemoveProperty_Click(sender, null);
            }
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            splitContainer1_SplitterMoved(sender, null);
        }

        private void collisionPolygonToolStripMenuItem_Click(object sender, EventArgs e)
        {
            collisionPolygonToolStripMenuItem.Checked = !collisionPolygonToolStripMenuItem.Checked;
        }

        private void splitContainer1_Panel1_MouseClick(object sender, MouseEventArgs e)
        {
            if (collisionPolygonToolStripMenuItem.Checked && e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                if (null != objectManager.SelectedObject)
                {
                    Size offset = Size.Empty;

                    offset.Width += splitContainer1.Panel1.AutoScrollPosition.X;
                    offset.Height += splitContainer1.Panel1.AutoScrollPosition.Y;

                    PointF loc = new PointF((int)((e.Location.X - offset.Width) / zoomLevel), (int)((e.Location.Y - offset.Height) / zoomLevel));

                    if(objectManager.SelectedObject.SelectedPoint == -1)
                    {
                        objectManager.SelectedObject.CollisionPoints.Add(loc);
                        objectManager.SelectedObject.SelectedPoint = objectManager.SelectedObject.CollisionPoints.Count - 1;
                    }
                    else
                    {
                        objectManager.SelectedObject.CollisionPoints.Insert(objectManager.SelectedObject.SelectedPoint + 1, loc);
                        objectManager.SelectedObject.SelectedPoint = objectManager.SelectedObject.SelectedPoint + 1;
                    }

                    if (objectManager.SelectedObject.CollisionPoints.Count >= 3)
                        isConvex = objectManager.SelectedObject.CollisionPoints.Count == 3 ? true : IsConvex(objectManager.SelectedObject.CollisionPoints);

                    saved = Saved_State.NOTSAVED;
                    Invalidate();
                }
            }
        }

        // Gets the full 360 degree angle between two vectors
        // Is actually using three verts, assuming they will share b as a point
        public static double AngleBetween(PointF a, PointF b, PointF c)
        {
            PointF vect1 = new PointF(b.X - a.X, b.Y - a.Y);
            PointF vect2 = new PointF(b.X - c.X, b.Y - c.Y);

            double atanA = Math.Atan2(vect1.Y, vect1.X);
            double atanB = Math.Atan2(vect2.Y, vect2.X);

            double result = (atanA - atanB) * (180.0 / Math.PI);

            if (result < 0)
                result += 360;

            return result;
        }

        public static bool IsConvex(List<PointF> verts)
        {
            //Testing how the polygon is wound 
            //Find the sum of the edges: (x2-x1)(y2+y1) 
            //if positive its clock-wise, 
            //if negitive: counter-clock-wise
            //point[0] = (5,0)   edge[0]: (6-5)(4+0) =   4
            //point[1] = (6,4)   edge[1]: (4-6)(5+4) = -18
            //point[2] = (4,5)   edge[2]: (1-4)(5+5) = -30
            //point[3] = (1,5)   edge[3]: (1-1)(0+5) =   0
            //point[4] = (1,0)   edge[4]: (5-1)(0+0) =   0
                                                     //---
                                                     //-44  counter-clockwise

            //find the sum of the edges to figure out how the polygon is wound
            int i, j, k;
            float sum = 0;
            for (i = 0; i < verts.Count; ++i)
            {                
                j = i + 1;
                if (j > verts.Count - 1)
                    j = 0;

                sum += (verts[j].X - verts[i].X) * (verts[j].Y + verts[i].Y);
            }

            double angle = 0;
            if (sum < 0) //if counter-clock-wise
            {
                for (i = 0; i < verts.Count; ++i)
                {
                    j = i - 1;
                    k = i + 1;

                    if (j < 0)
                        j = verts.Count - 1;
                    if (k > verts.Count - 1)
                        k = 0;

                    angle = AngleBetween(verts[j], verts[i], verts[k]);
                    if (angle > 180)
                        return false;
                }
                return true;
            }
            else //this is clock-wise
            {
                for (i = 0; i < verts.Count; ++i)
                {                
                    k = i - 1;
                    j = i + 1;

                    if (k < 0)
                        k = verts.Count - 1;
                    if (j > verts.Count - 1)
                        j = 0;

                    angle = AngleBetween(verts[j], verts[i], verts[k]);
                    if (angle > 180)
                        return false;
                }
                return true;
            }
        }

        private void splitContainer1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete && null != objectManager.SelectedObject)
            {
                if (objectManager.SelectedObject.SelectedPoint == -1)
                {
                    if (MessageBox.Show("Are you sure you want to delete this object?", "Confirmation", MessageBoxButtons.YesNo, MessageBoxIcon.Warning)
                        == System.Windows.Forms.DialogResult.Yes)
                    {
                        objectManager.RemoveObject(objectManager.SelectedObject);
                        objectManager.SelectedObject = null;
                        saved = Saved_State.NOTSAVED;
                        Invalidate();
                        FixViewList();
                        ButtonsEnabled(false);
                    }
                }
                else
                {
                    objectManager.SelectedObject.CollisionPoints.RemoveAt(objectManager.SelectedObject.SelectedPoint);
                    objectManager.SelectedObject.SelectedPoint = -1;

                    saved = Saved_State.NOTSAVED;
                    Invalidate();
                }
            }
        }

        private void splitContainer1_Panel1_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            //splitContainer1_KeyDown(sender, null);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {

        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (saved == Saved_State.NOTSAVED || (saved == Saved_State.NEVERSAVED && objectManager.ObjectList.Count > 0))
            {
                DialogResult result = MessageBox.Show("You have unsaved changes, you will loose any unsaved progress. Would you like to continue?",
                    "Confirmation", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);

                if (result == System.Windows.Forms.DialogResult.No)
                {
                    e.Cancel = true;                    
                }
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }


        private void FindCanvasSize()
        {
            if (objectManager.ObjectList.Count > 0)
            {
                objectManager.ObjectList.Sort(delegate(cObject lhs, cObject rhs) { return (rhs.Position.X + rhs.Image.Width).CompareTo((lhs.Position.X + lhs.Image.Width)); });
                greatestWidth = (int)((objectManager.ObjectList[0].Position.X + objectManager.ObjectList[0].Image.Width) * zoomLevel);

                objectManager.ObjectList.Sort(delegate(cObject lhs, cObject rhs) { return (rhs.Position.Y + rhs.Image.Height).CompareTo((lhs.Position.Y + lhs.Image.Height)); });
                greatestHeight = (int)((objectManager.ObjectList[0].Position.Y + objectManager.ObjectList[0].Image.Height) * zoomLevel);

                splitContainer1.Panel1.AutoScrollMinSize = new Size(greatestWidth, greatestHeight);
                Invalidate();
            }
        }

        private void helpToolStripButton_Click(object sender, EventArgs e)
        {

        }

        private void splitContainer1_Panel1_Scroll(object sender, ScrollEventArgs e)
        {
            Invalidate();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void numericUpDown_Zoom_ValueChanged(object sender, EventArgs e)
        {
            zoomLevel = (float)numericUpDown_Zoom.Value / 100.0f;
            FindCanvasSize();
            Invalidate();
            splitContainer1.Focus();
        }
    }
}
