#!/usr/bin/python

import json
import collections

chart_html_template = """<html>
  <head>
    <!--Load the AJAX API-->
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">

    // Load the Visualization API and the piechart package.
    google.load('visualization', '1', {'packages':['charteditor']});

    var wrapper;

    function init() {
      var data = new google.visualization.DataTable(%(jsonData)s);
      wrapper = new google.visualization.ChartWrapper({
        dataTable: data,
        options: %(options)s,
        containerId: 'visualization',
        chartType: 'LineChart'
      });
      wrapper.draw();
    }

    function openEditor() {
      // Handler for the "Open Editor" button.
      var editor = new google.visualization.ChartEditor();
      google.visualization.events.addListener(editor, 'ok',
        function() {
          wrapper = editor.getChartWrapper();
          wrapper.draw(document.getElementById('visualization'));
      });
      editor.openDialog(wrapper);
    }

    // Set a callback to run when the Google Visualization API is loaded.
    google.setOnLoadCallback(init);

    </script>
  </head>
  <body style="font-family: Arial;border: 0 none;">
    <input type='button' onclick='openEditor()' value='Customize Plot'>
    <div id='visualization'></div>
  </body>
</html>
"""


class LineChart:
    lineCount = 0
    cellRowData = collections.OrderedDict()
    cellColData = []
    lastFileName = ""

    @classmethod
    def generateHTML(cls, xvalues, yvalues, label, filename,
                            coltype="number", width=1440, height=900, title="",
                            xaxislimits=None, yaxislimits=None):

        if cls.lastFileName != filename:
            cls.reset()
            cls.lastFileName = filename

        cls.cellColData.append({"label": label, "type": coltype})

        for i, xval in enumerate(xvalues):
            cls.cellRowData.setdefault(xval,
                                     [None] * cls.lineCount).append(yvalues[i])

        for xval in set(cls.cellRowData.keys()) - set(xvalues):
            cls.cellRowData[xval].append(None)

        options = {"height": height, "width": width, "pointSize": 2,
                   "interpolateNulls": True}

        if title:
            options["title"] = title

        if xaxislimits:
            options["hAxis"] = {"minValue": xaxislimits[0],
                                "maxValue": xaxislimits[1]}

        if yaxislimits:
            options["vAxis"] = {"minValue": yaxislimits[0],
                                "maxValue": yaxislimits[1]}

        jsonData = {"cols": cls.cellColData, "rows": []}

        for xval, yvalues in cls.cellRowData.items():
            jsonData["rows"].append({"c": \
                                     [{"v": x} for x in [xval] + yvalues]})

        with open(filename, "w") as f:
            f.write(chart_html_template % {"jsonData": \
                                    json.dumps(jsonData, indent=1),
                                    "options": json.dumps(options)})

        cls.lineCount += 1


    @classmethod
    def reset(cls):
        # reset old data
        cls.cellRowData.clear()
        cls.cellColData = [{"label": "Something", "type": "number"}]
        cls.lastFileName = ""
        cls.lineCount = 0


if __name__ == "__main__":
    # Be careful! If x values of next lines are not in order, drawing order
    # of the points may be mixed up!
    LineChart.generateHTML([-10, 10, 4, 5, 6, 7],
                           [10, 200, 400, 460, 1120, 540],
                           "Expenses", "testfile.html")

    # Let's add another line to an existing line chart
    LineChart.generateHTML([4, 7, 6, 90, 11],
                           [1000, 1170, 660, 1030, 700],
                           "Sales", "testfile.html", yaxislimits=(-10, 1200),
                           title="Test plot")

    LineChart.generateHTML([3, 4, 5, 6, 7],
                           [10, 200, 400, 460, 1120, 540],
                           "Eppek", "testfile2.html")

    LineChart.generateHTML([3, 4, 5, 6, 7],
                           [1, 20, 40, 46, 112, 54],
                           "Pendir", "testfile2.html", title="Test Plot 2")
