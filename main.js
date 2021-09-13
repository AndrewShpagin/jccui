window.setInterval(() => {
    const body = fetch("/ping", {
        method: 'GET',
    }).then(body => {
        body.text().then(str => {
            if (str && str !== 'null') {
                const res = JSON.parse(str);
                if (res) {
                    for (const val of res) {
                        for (const [id, value] of Object.entries(val)) {
                            if (id === '$$$evaluate') {
                                eval(value);
                            } else
                            if (id === 'console') {
                                console.log(value);
                            } else {
                                console.log('dom:', id, value);
                                const el = document.getElementById(id);
                                if (el) {
                                    el.innerHTML = value;
                                }
                            }
                        }
                    }
                }
            }
        });
    });
}, pingTimeMs);
async function sendObject(object) {
    try {
        const body = await fetch("/exchange", {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(object)
        });
        const str = await body.text();
        const res = JSON.parse(str);
        return res || {};
    } catch (error) {
        console.log(error);
    }
    return {};
}