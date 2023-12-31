// Ваша функция submitWord в файле script.js

function submitWord() {
    const word = document.getElementById('word').value;
    // Відправте слово на сервер для обробки
    fetch('/check_word', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: word,
    })
        .then(response => response.json())
        .then(data => {
            console.log(data);
            // Виведіть результат AST на сторінці
           // document.getElementById('astResult').innerText = JSON.stringify(data, null, 2);

            console.log(data.ast);
            console.log(data.message);

            // Малюємо AST-дерево
            drawTree(data.ast);

            // Отображаем сообщение на странице
            document.getElementById('messageContainer').innerText = "Result - " + data.message;
        });
}

var margin = {
        top: 20,
        right: 120,
        bottom: 20,
        left: 120
    },
    width = 960 ;
    height = 1500 ;
var root = {
    "name": "Eve",
    "value": 15,
    "type": "black",
    "level": "yellow",
    "children": [
        {
            "name": "Cain",
            "value": 10,
            "type": "grey",
            "level": "red"
        }
    ],
};
var i = 0,
    duration = 700,
    rectW = 40,
    rectH = 20;

var tree = d3.layout.tree().nodeSize([70, 40]);
var diagonal = d3.svg.diagonal()
    .projection(function (d) {
        return [d.x + rectW / 2, d.y + rectH / 2];
    });

var svg = d3.select("#treeContainer").append("svg").attr("width", 960).attr("height", 800)
    .call(zm = d3.behavior.zoom().scaleExtent([1,3]).on("zoom", redraw)).append("g")
    .attr("transform", "translate(" + 480 + "," + 0 + ")");

zm.translate([480, 0]);

root.x0 = 0;
root.y0 = height / 2;
root.children.forEach(collapse);
update(root);

d3.select("#treeContainer").style("height", "1500px");
function drawTree(data) {
     root = data;

    zm.translate([480, 0]);

    root.x0 = 0;
    root.y0 = height / 2;
    if (root.children) {
        root.children.forEach(collapse);
    }
    update(root);

    d3.select("#treeContainer").style("height", "1500px");
//necessary so that zoom knows where to zoom and unzoom from
}

function collapse(d) {
    if (d.children) {
        d._children = d.children;
        d._children.forEach(collapse);
        d.children = null;
    }
}

function update(source) {

    // Compute the new tree layout.
    var nodes = tree.nodes(root).reverse(),
        links = tree.links(nodes);
    // Normalize for fixed-depth.
    nodes.forEach(function (d) {
        d.y = d.depth * 60;
    });

    // Update the nodes…
    var node = svg.selectAll("g.node")
        .data(nodes, function (d) {
            return d.id || (d.id = ++i);
        });

    // Enter any new nodes at the parent's previous position.
    var nodeEnter = node.enter().append("g")
        .attr("class", "node")
        .attr("transform", function (d) {
            return "translate(" + source.x0 + "," + source.y0 + ")";
        })
        .on("click", click);

    nodeEnter.append("rect")
        .attr("width", rectW)
        .attr("height", rectH)
        .attr("stroke", "black")
        .attr("stroke-width", 1)
        .style("fill", function (d) {
            return d._children ? "lightsteelblue" : "#fff";
        });

    nodeEnter.append("text")
        .attr("x", rectW / 2)
        .attr("y", rectH / 2)
        .attr("dy", ".35em")
        .attr("text-anchor", "middle")
        .text(function (d) {
            return d.value;
        });

    // Transition nodes to their new position.
    var nodeUpdate = node.transition()
        .duration(duration)
        .attr("transform", function (d) {
            return "translate(" + d.x + "," + d.y + ")";
        });

    nodeUpdate.select("rect")
        .attr("width", rectW)
        .attr("height", rectH)
        .attr("stroke", "black")
        .attr("stroke-width", 1)
        .style("fill", function (d) {
            return d._children ? "lightsteelblue" : "#fff";
        });

    nodeUpdate.select("text")
        .style("fill-opacity", 1);

    // Transition exiting nodes to the parent's new position.
    var nodeExit = node.exit().transition()
        .duration(duration)
        .attr("transform", function (d) {
            return "translate(" + source.x + "," + source.y + ")";
        })
        .remove();

    nodeExit.select("rect")
        .attr("width", rectW)
        .attr("height", rectH)
        //.attr("width", bbox.getBBox().width)""
        //.attr("height", bbox.getBBox().height)
        .attr("stroke", "black")
        .attr("stroke-width", 1);

    nodeExit.select("text");

    // Update the links…
    var link = svg.selectAll("path.link")
        .data(links, function (d) {
            return d.target.id;
        });

    // Enter any new links at the parent's previous position.
    link.enter().insert("path", "g")
        .attr("class", "link")
        .attr("x", rectW / 2)
        .attr("y", rectH / 2)
        .attr("d", function (d) {
            var o = {
                x: source.x0,
                y: source.y0
            };
            return diagonal({
                source: o,
                target: o
            });
        });

    // Transition links to their new position.
    link.transition()
        .duration(duration)
        .attr("d", diagonal);

    // Transition exiting nodes to the parent's new position.
    link.exit().transition()
        .duration(duration)
        .attr("d", function (d) {
            var o = {
                x: source.x,
                y: source.y
            };
            return diagonal({
                source: o,
                target: o
            });
        })
        .remove();

    // Stash the old positions for transition.
    nodes.forEach(function (d) {
        d.x0 = d.x;
        d.y0 = d.y;
    });
}

// Toggle children on click.
function click(d) {
    if (d.children) {
        d._children = d.children;
        d.children = null;
    } else {
        d.children = d._children;
        d._children = null;
    }
    update(d);
}

//Redraw for zoom
function redraw() {
    //console.log("here", d3.event.translate, d3.event.scale);
    svg.attr("transform",
        "translate(" + d3.event.translate + ")"
        + " scale(" + d3.event.scale + ")");
}
