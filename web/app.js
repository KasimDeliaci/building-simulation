/****************************************************************
 * Viewer v4 – Sol bina + sağda 4 bağımsız sahne
 ****************************************************************/
import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.161/build/three.module.js';

/* ---------- Renk tablo­su ---------- */
const C={idle:0x444444,work:0x1e90ff,done:0x00cc66,
         vinç:0x00c8de,hat:0xffd700,dekor:0x1e90ff,zemin:0xc653ff};

/* ---------- Sol: bina sahnesi ---------- */
const CUBE=1.5,GX=2,GY=1.75,F=10,A=4;
const lR=new THREE.WebGLRenderer({canvas:document.querySelector('#cLeft'),antialias:true});
const lS=new THREE.Scene(); lS.background=new THREE.Color(0x111111);
const camL=new THREE.PerspectiveCamera(45,1,.1,100);
const midY=(F-1)*GY/2; camL.position.set(7,midY,22); camL.lookAt(0,midY,0);
lS.add(new THREE.AmbientLight(0xffffff,.9));

function sprite(txt){
  const cvs=document.createElement('canvas'); cvs.width=cvs.height=64;
  const g=cvs.getContext('2d');
  g.fillStyle='#fff'; g.font='26px sans-serif'; g.textAlign='center'; g.textBaseline='middle';
  g.fillText(txt,32,32);
  return new THREE.Sprite(new THREE.SpriteMaterial({map:new THREE.CanvasTexture(cvs),depthTest:false}));
}
const cubes={};
for(let f=0;f<F;f++){
 for(let a=0;a<A;a++){
   const mesh=new THREE.Mesh(new THREE.BoxGeometry(CUBE,CUBE,CUBE),
                             new THREE.MeshStandardMaterial({color:C.idle}));
   mesh.position.set(a*GX-GX*1.5,f*GY,0);
   const tag=sprite(`${f+1}.${a+1}`);
   tag.position.set(mesh.position.x,mesh.position.y,CUBE*0.75);
   lS.add(mesh,tag); cubes[`F${f}A${a}`]=mesh;
 }}

/* ---------- Sağ: 4 bağımsız sahne ---------- */
const slots=[];                         // {ren,scene,cube,cam,note,busy}
document.querySelectorAll('#right .slot').forEach(div=>{
  const canvas=div.querySelector('canvas'), note=div.querySelector('.note');
  const ren=new THREE.WebGLRenderer({canvas,antialias:true});
  const scene=new THREE.Scene(); scene.background=new THREE.Color(0x111111);
  const cam=new THREE.PerspectiveCamera(45,1,.1,50);
  cam.position.set(5,6,12); cam.lookAt(0,0,0);
  scene.add(new THREE.AmbientLight(0xffffff,.9));
  const cube=new THREE.Mesh(new THREE.BoxGeometry(2.75,2.75,2.75),
                            new THREE.MeshStandardMaterial({color:C.idle}));
  cube.rotation.y=Math.PI/4; scene.add(cube);
  slots.push({ren,scene,cam,cube,note,busy:false});
});

/* ---------- Adım -> renk + kısa yazı ---------- */
function style(step){
  if(step.includes('dış duvar'))                            return {c:C.vinç ,txt:'Dış Duvar'};
  if(step.includes('Vinç'))                                 return {c:C.vinç ,txt:'İskelet'};
  if(step.includes('Dış-iskelet'))                          return {c:C.hat  ,txt:'Dış→İç'}
  if(step.startsWith('Elektrik-su'))                        return {c:C.hat  ,txt:'Elektrik/Su'};
  if(step.startsWith('Final'))                              return {c:C.hat  ,txt:'Final'};
  if(step.startsWith('Sıva'))                               return {c:C.dekor,txt:'Sıva'};
  if(step.startsWith('Boya'))                               return {c:C.dekor,txt:'Boya'};
  if(step.includes('Fayans')||step.includes('parkeler'))    return {c:C.zemin,txt:'Zemin'};
  if(step.includes('Mobilya'))                              return {c:C.zemin,txt:'Mobilya'};
  return {c:C.work,txt:'?'}; }

/* ---------- CSV yükle ---------- */
fetch('run.csv').then(r=>r.text()).then(txt=>{
  const ev=txt.trim().split('\n').slice(1).map(l=>{
    const[sec,f,a,step]=l.split(',');
    const s=style(step);
    return{sec:+sec,key:`F${f}A${a}`,step,color:s.c,label:s.txt};
  }).sort((a,b)=>a.sec-b.sec);
  animate(ev);
});

const info=document.getElementById('info');

/* ---------- Animasyon döngüsü ---------- */
function animate(events){
  let t0=null;
  function loop(ts){
    if(!t0)t0=ts;
    const sim=((ts-t0)/1000)|0;

    while(events.length && events[0].sec<=sim){
      const e=events.shift(), small=cubes[e.key]; if(!small) continue;

      /* Sol bina kubu renk */
      small.material.color.setHex(e.step.startsWith('Final')?C.done:C.work);

      /* Sağ slot seçimi */
      const slot=slots.find(s=>!s.busy) || slots[0];
      slot.busy=true;
      slot.cube.material.color.setHex(e.color);
      slot.note.textContent=`${+e.key[1]+1}.${+e.key[3]+1}  ${e.label}`;

      /* Vinç / Asansör satırı */
      if(e.step.startsWith('Vinçte'))
          info.textContent='Vinçte: '+e.step.replace('Vinçte ','');
      else if(e.step.startsWith('Asansörde'))
          info.textContent='Asansörde: '+e.step.replace('Asansörde ','');
      else
          info.textContent=e.step;

      /* 1 s sonra slot’u boşalt */
      setTimeout(()=>{
        slot.busy=false;
        slot.cube.material.color.setHex(C.idle);
        slot.note.textContent='';
      },1000);
    }

    /* Render */
    lR.render(lS,camL);
    slots.forEach(s=>s.ren.render(s.scene,s.cam));

    requestAnimationFrame(loop);
  }
  requestAnimationFrame(loop);
}

/* ---------- Pencere yeniden boyut ---------- */
function resize(){
  const w=innerWidth/2,h=innerHeight;
  lR.setSize(w,h); camL.aspect=w/h; camL.updateProjectionMatrix();
  slots.forEach(s=>{
    s.ren.setSize(w/2,h/2);
    s.cam.aspect=(w/2)/(h/2); s.cam.updateProjectionMatrix();
  });
}
resize(); window.addEventListener('resize',resize);
