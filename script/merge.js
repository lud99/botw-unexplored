const seeds = require("./zeldadungeon.json");
const positions = require("./pos.json");

const { writeFileSync} = require("fs");

let out = seeds.slice();

for (let i = 0; i < seeds.length; i++)
{
    for (let j = 0; j < positions.length; j++)
    {
        let seedid = seeds[i].image.split("/")[seeds[i].image.split("/").length - 1].replace("200px-", "").replace("225px-", "").
            replace("Korok", "Seed").replace(".png", "").replace(".jpg", "");

        if (seedid == positions[i].id)
        {
            out[i].x = positions[i].x;
            out[i].y = positions[i].y;
            out[i].id = parseInt(seedid.replace("Seed", ""));
        }
    }
}

writeFileSync("koroks.json", JSON.stringify(out, null, 4));