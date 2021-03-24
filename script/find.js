const real = require("./p.json");
const dungeon = require("./koroks.json");
const { writeFileSync, readFileSync } = require("fs");


const distance = (x1, y1, x2, y2) => Math.sqrt((x2-x1)**2 + (y2-y1)**2);

res = [];

for (let i = 0; i < 900; i++) {
    let dists = [];

    for (let d = 0; d < 900; d++) {
        dists.push(distance(real[i].x, -real[i].y, dungeon[d].x, dungeon[d].y));
    }

    let closest = new Float64Array(dists).sort()[0];
    let theId = dungeon[dists.indexOf(closest)].id;

    if (theId == undefined) console.log(i, closest)

    //console.log(closest, i, theId);
    res.push({ ...real[i], id: theId, originalArrayEntry: i });
}

let map = {

};

for (let i = 0; i < 900; i++)
{
    if (!map[i])
    {
        let f = res.find(a => a.id == i);
        map[i] = f;
    } else
    {
        console.log("Duplicate: ", map[i].id)
    }
}

const c = readFileSync("./str.txt", "utf-8");
let f = c.split("\n").map((e, i) => {
    return e.replace(")", ", " + res[i].id + ")");
})
f = f.join("\n");

writeFileSync("final.txt", f);
writeFileSync("final.json", JSON.stringify(res, null, 4));