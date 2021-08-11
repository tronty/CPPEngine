

import java.applet.Applet;
import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Choice;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.Label;
import java.awt.Panel;
import java.awt.Scrollbar;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.EventObject;
import java.util.StringTokenizer;

public class PoincareApplet
  extends Applet
  implements ActionListener, AdjustmentListener, ItemListener
{
  TileParameters par = new TileParameters();
  boolean interactive;
  PoincareDisk disk;
  Panel controlPanel = new Panel();
  Button drawButton = new Button("Draw");
  Choice quasiChoice = new Choice();
  Choice altChoice = new Choice();
  Label n_label = new Label("", 1);
  Scrollbar n_scrollbar;
  Label k_label = new Label("", 1);
  Scrollbar k_scrollbar;
  Label skip_label = new Label("", 1);
  Scrollbar skip_scrollbar;
  Label layers_label = new Label("", 1);
  Scrollbar layers_scrollbar;
  static String[] colorName = { "black", "blue", "cyan", "darkGray", "gray", "green", "lightGray", "magenta", "orange", "pink", "red", "white", "yellow" };
  static Color[] constColor = { Color.black, Color.blue, Color.cyan, Color.darkGray, Color.gray, Color.green, Color.lightGray, Color.magenta, Color.orange, Color.pink, Color.red, Color.white, Color.yellow };
  
  public void init()
  {
    getParameters();
    disk = new PoincareDisk(par);
    initializePanels();
    checkPars();
    disk.init();
    disk.repaint();
  }
  
  private void getParameters()
  {
    String str = getParameter("n");
    par.n = (str == null ? 5 : Integer.parseInt(str));
    str = getParameter("k");
    par.k = (str == null ? 4 : Integer.parseInt(str));
    str = getParameter("quasiregular");
    par.quasiregular = (str != null);
    str = getParameter("layers");
    par.layers = (str == null ? 4 : Integer.parseInt(str));
    str = getParameter("skipNumber");
    par.skipNumber = (str == null ? 1 : Integer.parseInt(str));
    str = getParameter("bgColor");
    if (str != null) {
      par.bgColor = parseColor(str);
    }
    if (par.bgColor == null) {
      par.bgColor = getBackground();
    }
    str = getParameter("diskColor");
    if (str != null) {
      par.diskColor = parseColor(str);
    }
    if (par.diskColor == null) {
      par.diskColor = Color.black;
    }
    str = getParameter("fill");
    par.fill = (str == null);
    str = getParameter("outline");
    par.outline = (str == null);
    str = getParameter("grayScale");
    par.grayScale = (str != null);
    str = getParameter("alternating");
    par.alternating = ((str != null) && (par.k % 2 != 1));
    str = getParameter("interactive");
    interactive = (str == null);
  }
  
  private void initializePanels()
  {
    setLayout(new BorderLayout(10, 10));
    setFont(new Font("TimesRoman", 0, 12));
    controlPanel.setLayout(new GridLayout(11, 1, 5, 5));
    drawButton.addActionListener(this);
    controlPanel.add(drawButton);
    quasiChoice.addItem("Regular");
    quasiChoice.addItem("Quasi");
    quasiChoice.addItemListener(this);
    controlPanel.add(quasiChoice);
    controlPanel.add(n_label);
    n_scrollbar = new Scrollbar(0, par.n, 0, 3, 21);
    n_scrollbar.addAdjustmentListener(this);
    controlPanel.add(n_scrollbar);
    controlPanel.add(k_label);
    k_scrollbar = new Scrollbar(0, par.k, 0, 3, 21);
    k_scrollbar.addAdjustmentListener(this);
    controlPanel.add(k_scrollbar);
    controlPanel.add(skip_label);
    skip_scrollbar = new Scrollbar(0, par.skipNumber, 0, 1, 21);
    skip_scrollbar.addAdjustmentListener(this);
    controlPanel.add(skip_scrollbar);
    controlPanel.add(layers_label);
    layers_scrollbar = new Scrollbar(0, par.layers + 1, 0, 0, 6);
    layers_scrollbar.addAdjustmentListener(this);
    controlPanel.add(layers_scrollbar);
    altChoice.addItem("Random");
    altChoice.addItem("Alternating");
    altChoice.addItemListener(this);
    controlPanel.add(altChoice);
    if (interactive) {
      add("West", controlPanel);
    }
    Panel localPanel = new Panel();
    localPanel.setLayout(new GridLayout(1, 1, 10, 10));
    localPanel.add(disk);
    add(localPanel);
  }
  
  private void checkPars()
  {
    par.checkPars();
    n_label.setText("n = " + par.n);
    n_scrollbar.setValue(par.n);
    k_label.setText("k = " + par.k);
    k_scrollbar.setValue(par.k);
    skip_label.setText("skip=" + par.skipNumber);
    skip_scrollbar.setValue(par.skipNumber);
    skip_scrollbar.setMaximum((par.n + 1) / 2);
    layers_label.setText("layers=" + par.layers);
    layers_scrollbar.setValue(par.layers);
    if (par.quasiregular) {
      quasiChoice.select("Quasi");
    } else {
      quasiChoice.select("Regular");
    }
    if (par.alternating) {
      altChoice.select("Alternating");
    } else {
      altChoice.select("Random");
    }
    altChoice.setEnabled(par.k % 2 == 0);
  }
  
  Color parseColor(String paramString)
  {
    if ((paramString == null) || ("none".equals(paramString)) || ("0".equals(paramString))) {
      return null;
    }
    for (int i = 0; i < colorName.length; i++) {
      if (colorName[i].equals(paramString)) {
        return constColor[i];
      }
    }
    try
    {
      return new Color(Integer.parseInt(paramString, 16));
    }
    catch (NumberFormatException localNumberFormatException1)
    {
      StringTokenizer localStringTokenizer = new StringTokenizer(paramString, ",");
      if (!localStringTokenizer.hasMoreTokens()) {
        return null;
      }
      try
      {
        float f1 = (float)(Integer.parseInt(localStringTokenizer.nextToken()) / 360.0D);
        if (!localStringTokenizer.hasMoreTokens()) {
          return null;
        }
        float f2 = (float)(Integer.parseInt(localStringTokenizer.nextToken()) / 100.0D);
        if (!localStringTokenizer.hasMoreTokens()) {
          return null;
        }
        float f3 = (float)(Integer.parseInt(localStringTokenizer.nextToken()) / 100.0D);
        return new Color(Color.HSBtoRGB(f1, f2, f3));
      }
      catch (NumberFormatException localNumberFormatException2) {}
    }
    return null;
  }
  
  public void actionPerformed(ActionEvent paramActionEvent)
  {
    String str = paramActionEvent.getActionCommand();
    if (str.equals("Draw"))
    {
      checkPars();
      disk.init();
      disk.repaint();
    }
  }
  
  public void itemStateChanged(ItemEvent paramItemEvent)
  {
    par.quasiregular = (quasiChoice.getSelectedIndex() == 1);
    par.alternating = (altChoice.getSelectedIndex() == 1);
    checkPars();
  }
  
  public void adjustmentValueChanged(AdjustmentEvent paramAdjustmentEvent)
  {
    if (paramAdjustmentEvent.getSource() == n_scrollbar)
    {
      par.n = paramAdjustmentEvent.getValue();
    }
    else if (paramAdjustmentEvent.getSource() == k_scrollbar)
    {
      par.k = paramAdjustmentEvent.getValue();
      if (par.k == 3) {
        par.n = Math.max(par.n, 7);
      } else if (par.k == 4) {
        par.n = Math.max(par.n, 5);
      } else if (par.k < 7) {
        par.n = Math.max(par.n, 4);
      } else {
        par.n = Math.max(par.n, 3);
      }
    }
    else if (paramAdjustmentEvent.getSource() == skip_scrollbar)
    {
      par.skipNumber = paramAdjustmentEvent.getValue();
    }
    else if (paramAdjustmentEvent.getSource() == layers_scrollbar)
    {
      par.layers = paramAdjustmentEvent.getValue();
    }
    checkPars();
  }
  
  public PoincareApplet() {}
}


