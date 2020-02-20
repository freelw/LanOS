const fs = require('fs');

const file_list = [
    'hello_fs',
    'test',
    'lan_sh',
];

function zero_buffer(size) {
    let ret = Buffer.alloc(size);
    for (let i = 0; i < size; ++ i) {
        ret[i] = 0;
    }
    return ret;
}

async function main() {
    const vfd_path = `${__dirname}/../a.vfd`;
    const vfd_content = fs.readFileSync(vfd_path);
    console.log('vfd_content length : ', vfd_content.length);
    const vfd_length = vfd_content.length;
    let fs_header = zero_buffer(4);
    const file_cnt = file_list.length;
    fs_header.writeInt32LE(file_cnt);
    let file_buffer = zero_buffer(0);
    for (let i = 0; i < file_cnt; ++ i) {
        const path = `${__dirname}/../root_fs/${file_list[i]}`;
        const content = fs.readFileSync(path);
        const content_length = content.length;
        console.log(`path ${path} : ${content_length}`);
        const file_length_buffer = zero_buffer(4);
        const file_name_buffer = zero_buffer(8);
        file_length_buffer.writeInt32LE(content_length);
        Buffer.from(file_list[i]).copy(file_name_buffer);
        fs_header = Buffer.concat([fs_header, file_length_buffer, file_name_buffer]);
        const file_pad = zero_buffer(20*1024-content_length);
        file_buffer = Buffer.concat([file_buffer, content, file_pad]);
    }
    const fs_header_pad = zero_buffer(124-fs_header.length);
    fs_header = Buffer.concat([fs_header, fs_header_pad]);
    console.log('fs_header length : ', fs_header.length);
    const start = 0x5df84; // 0x90000-200k-124 在内存中的位置应该是0x5df84-0x200+0x10000=0x6dd84
    const p0 = vfd_content.slice(0, start);
    const p1 = Buffer.concat([fs_header, file_buffer]);
    const p2 = vfd_content.slice(start+p1.length);
    const final_buffer = Buffer.concat([p0, p1, p2]);
    fs.writeFileSync(vfd_path, final_buffer);
    process.exit();
}
main();