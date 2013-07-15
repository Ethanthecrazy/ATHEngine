using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsFormsApplication1
{
    public class ObjectManager
    {
        private cObject selectedObject;
        public cObject SelectedObject
        {
            get { return selectedObject; }
            set { selectedObject = value; }
        }

        private List<cObject> objectList;
        public List<cObject> ObjectList
        {
            get { return objectList; }
            set { objectList = value; }
        }

        public ObjectManager()
        {
            selectedObject = null;
            objectList = new List<cObject>();
        }

        public void AddObject(cObject obj)
        {
            objectList.Add(obj);
        }

        public void RemoveObject(cObject obj)
        {
            objectList.Remove(obj);
        }
    }
}
